#include "RemoteControlPrivatePCH.h"

#include "RemoteControlModule.h"
#include "RemoteControl.h"
#include "Runtime/Core/Public/Features/IModularFeatures.h"
#include "SlateBasics.h"
#include "ScreenRendering.h"
#include "RenderCore.h"
#include "RHIStaticStates.h"
#include "ISettingsModule.h"
#include "RendererInterface.h"
#include "SlateApplication.h"
#include "PipelineStateCache.h"
#include "Runtime/Launch/Resources/Version.h"

IMPLEMENT_MODULE(FRemoteControl, RemoteControl);

#define LOCTEXT_NAMESPACE "RemoteControl"

FRemoteControl::FRemoteControl()
{
	bIsBroadcasting = false;
	bPendingStart = false;
	StartTimeCounter = 0.f;
	BroadcastResolution = FVector2D(512, 512);
	SocketUdp = nullptr;
	UDPReceiver = nullptr;
	UDPSender = nullptr;

	LastHeadTransform = FRotator::ZeroRotator;
}

void FRemoteControl::StartupModule()
{
	// Register settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	
	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings("Editor", "Plugins", "RemoteControl",
			LOCTEXT("RemoteControlSettingsName", "Remote Control"),
			LOCTEXT("RemoteControlSettingsDescription", "Remotely control the game from your mobile device."),
			GetMutableDefault< URemoteControlSettings >()
			);
	}
	
	// Register delegate for ticker callback
	TickDelegate = FTickerDelegate::CreateRaw(this, &FRemoteControl::Tick);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
	
	const auto& Settings = *GetDefault< URemoteControlSettings >();
}

void FRemoteControl::ShutdownModule()
{
	// Unregister settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Editor", "General", "RemoteControl");
	}

	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	
	// Stop broadcasting at shutdown
	StopBroadcast();
}

void FRemoteControl::StartUDP()
{
	// @todo: Not hurts :)
	StopUDP();

	const auto& Settings = *GetDefault< URemoteControlSettings >();

	auto SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	SocketUdp = SocketSubsystem->CreateSocket(NAME_DGram, TEXT("Remote Control"), true);
	check(SocketUdp != nullptr);
	
	SocketUdp->SetNonBlocking(true);
	SocketUdp->SetReuseAddr(true);
	SocketUdp->SetBroadcast(false);

	int32 BufferSize = 65535;
	int32 OutNewSize;
	SocketUdp->SetReceiveBufferSize(BufferSize, OutNewSize);
	SocketUdp->SetSendBufferSize(BufferSize, OutNewSize);

	TSharedPtr<FInternetAddr> MyAddr = SocketSubsystem->CreateInternetAddr();
	MyAddr->SetAnyAddress();
	MyAddr->SetPort(Settings.Port);
	SocketUdp->Bind(*MyAddr);

	UDPReceiver = new FRemoteUDPReceiver(SocketUdp, TEXT("Remote Control - UDP Receiver"));
	UDPReceiver->OnReceived().BindRaw(this, &FRemoteControl::OnUDPReceived);

	UDPSender = new FRemoteUDPSender(SocketUdp, TEXT("Remote Control - UDP Sender"));
}

void FRemoteControl::StopUDP()
{
	if (UDPSender)
	{
		UDPSender->Stop();
		UDPSender = nullptr;
	}

	if (UDPReceiver)
	{
		UDPReceiver->Stop();
		UDPReceiver = nullptr;
	}

	if (SocketUdp)
	{
		SocketUdp->Close();
	}

	if (SocketUdp)
	{
		auto SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		if (SocketSubsystem)
		{
			SocketSubsystem->DestroySocket(SocketUdp);
		}
		SocketUdp = nullptr;
	}
}

FRemoteControl::FViewportInfo FRemoteControl::GetViewportInfo(int32 ControllerId)const
{
	if (PlayerInfos.Contains(ControllerId))
	{
		return PlayerInfos[ControllerId].ViewportInfo;
	}

	return FViewportInfo();
}

void FRemoteControl::SendVideoFrameBuffer(int32 ControllerId, const uint8* VideoFrameBuffer, uint32 BufferSize, int32 Width, int32 Height)
{
	uint8* BufferData = new uint8[BufferSize];
	FMemory::Memcpy(BufferData, VideoFrameBuffer, BufferSize);

	SyncActions();
	
	const auto& Settings = *GetDefault< URemoteControlSettings >();
	auto Task = new FAsyncTask<class FRemoteActionImageCompress>(this, ControllerId, BufferData, BufferSize, Width, Height, Settings.GetRealQuality());
	Actions.Add(Task);
	
	Task->StartBackgroundTask();
}

void FRemoteControl::OnUDPReceived(const FArrayReaderPtr& Data, const FIPv4Endpoint& Addr)
{
	if (bIsBroadcasting)
	{
		for (int32 Index = 0; Index < Communications.Num(); Index++)
		{
			auto Comm = Communications[Index];
			if (Comm && Comm->IsUdp() && Comm->IsConnected() && Comm->GetRemoteEndPoint() == Addr)
			{
				Comm->OnReceived(false, Data);
			}
		}
	}
}

void FRemoteControl::InitCommunication(FString Ip)
{
	const auto& Settings = *GetDefault< URemoteControlSettings >();

	FRemoteCommunication* FirstCommunication = nullptr;
	
	if (Ip.IsEmpty() && Settings.Communication == ERemoteControlCommunication::USB)
	{
		FirstCommunication = (new FRemoteCommunication(this, /*bIsTCP=*/true, TEXT("127.0.0.1"), Settings.Port));
		Communications.Add(FirstCommunication);
	}
	else if (Ip.IsEmpty())
	{
		FirstCommunication = (new FRemoteCommunication(this, /*bIsTCP=*/false, Settings.IpAddress, Settings.Port));
		Communications.Add(FirstCommunication);
	}
	else
	{
		FirstCommunication = (new FRemoteCommunication(this, /*bIsTCP=*/false, Ip, Settings.Port));
		Communications.Add(FirstCommunication);
	}

	// Don't need this in the shipping build.
#if !UE_BUILD_SHIPPING
	if (Settings.bEnableAdditionalControllers)
	{
		for (int32 Index = 0; Index < Settings.AdditionalControllers.Num(); Index++)
		{
			const FString Ip2 = Settings.AdditionalControllers[Index];
			if (!Ip2.IsEmpty())
			{
				//@todo: Additional controllers doesn't support USB communication.
				auto Comm = (new FRemoteCommunication(this,  /*bIsTCP=*/false, Ip2, Settings.Port));
				Communications.Add(Comm);
			}
		}
	}
#endif
}

class FRemoteCommunication* FRemoteControl::GetCommunication(int32 ControllerId) const
{
	if (Communications.Num() > 1)
	{
		// Multiplayer. Get communication by controller id
		for (int32 Index = 0; Index < Communications.Num(); Index++)
		{
			if (Communications[Index]->GetControllerId() == ControllerId)
			{
				return Communications[Index];
			}
		}
	}
	else if (Communications.Num() > 0)
	{
		// Single player. We don't care controller id for single player.
		return Communications[0];
	}

	return nullptr;
}

void FRemoteControl::SyncActions()
{
	FScopeLock Lock(&SyncActionsSection);

	for (int32 Index = 0; Index < Actions.Num(); Index++)
	{
		auto Action = Actions[Index];
		if (Action)
		{
			Action->EnsureCompletion(/*bDoWorkOnThisThreadIfNotStarted=*/true);
			delete Action;
		}

		Actions.RemoveAt(Index);
		--Index;
	}
}

void FRemoteControl::OnActionReceived(int32 ControllerId, int32 Action, const TArray<uint8> Data)
{
	FScopeLock Lock(&ReceivedActionsLock);
	ReceivedActions.Add(FReceivedAction(Communications.Num() > 1 ? ControllerId : 0, Action, Data));
}

FString FRemoteControl::GetADBPath()const
{
#if !UE_BUILD_SHIPPING
	TCHAR AndroidDirectory[32768] = { 0 };
	FPlatformMisc::GetEnvironmentVariable(TEXT("ANDROID_HOME"), AndroidDirectory, 32768);
	
	FString ADBPath = TEXT("");
	if (AndroidDirectory[0] != 0)
	{
#if PLATFORM_WINDOWS
		ADBPath = FString::Printf(TEXT("%s\\platform-tools\\adb.exe"), AndroidDirectory);
#else
		ADBPath = FString::Printf(TEXT("%s/platform-tools/adb"), AndroidDirectory);
#endif
	}
	
	return ADBPath;
#else
	return TEXT("");
#endif
}

void FRemoteControl::OpenADBConnection()
{
#if !UE_BUILD_SHIPPING
	const auto& Settings = *GetDefault< URemoteControlSettings >();

	FString ADBPath = GetADBPath();
	if (!ADBPath.IsEmpty() && FPaths::FileExists(*ADBPath))
	{
		const FString CommandLine = FString::Printf(TEXT("forward tcp:%i tcp:%i"), Settings.Port, Settings.Port);;

		// execute the command
		int32 ReturnCode;
		FString OutStdOut;
		FString OutStdErr;
		FPlatformProcess::ExecProcess(*ADBPath, *CommandLine, &ReturnCode, &OutStdOut, &OutStdErr);

		if (ReturnCode == 0)
		{
			// Success
		}
	}
#endif
}

void FRemoteControl::CloseADBConnection()
{
#if !UE_BUILD_SHIPPING
	const auto& Settings = *GetDefault< URemoteControlSettings >();

	FString ADBPath = GetADBPath();
	if (!ADBPath.IsEmpty() && FPaths::FileExists(*ADBPath))
	{
		const FString CommandLine = FString::Printf(TEXT("forward --remove tcp:%i"), Settings.Port);

		// execute the command
		int32 ReturnCode;
		FString OutStdOut;
		FString OutStdErr;
		FPlatformProcess::ExecProcess(*ADBPath, *CommandLine, &ReturnCode, &OutStdOut, &OutStdErr);

		if (ReturnCode == 0)
		{
			// Success
		}
	}
#endif
}

void FRemoteControl::SendAction(int32 ControllerId, uint32 InAction)
{
	class FRemoteCommunication* const Comm = GetCommunication(ControllerId);
	if (Comm && Comm->IsConnected())
	{
		Comm->SendAction(InAction);
	}
}

void FRemoteControl::SendAction(int32 ControllerId, uint32 InAction, const FBufferArchive& InData)
{
	class FRemoteCommunication* const Comm = GetCommunication(ControllerId);
	if (Comm && Comm->IsConnected())
	{
		Comm->SendAction(InAction, InData);
	}
}

void FRemoteControl::OnControllerChanged(FRemoteCommunication* Comm, int32 OldControllerId, int32 NewControllerId)
{
	bool bReplaced = false;

	for (int32 Index = 0; Index < Communications.Num(); Index++)
	{
		auto Item = Communications[Index];
		if (Item != Comm)
		{
			if (Item->GetControllerId() == NewControllerId)
			{
				if (!bReplaced)
				{
					Item->SetControllerId(OldControllerId);
					bReplaced = true;
				}
				else
				{
					//@todo: This should not be happen but will not hurt anyone.
					Item->SetControllerId(0);
				}
			}
		}
	}
}

bool FRemoteControl::FindViewportGeometry(TSharedPtr<SWindow> Window, FGeometry& OutGeometry)const
{
	if (Window.IsValid())
	{
		return FindViewportGeometryInternal(Window->GetWindowGeometryInWindow(), Window, OutGeometry);
	}

	return false;
}

bool FRemoteControl::FindViewportGeometryInternal(const FGeometry& Geometry, TSharedPtr<SWidget> Widget, FGeometry& OutGeometry)const
{
	FArrangedChildren ArrangedChildren(EVisibility::Visible);
	Widget->ArrangeChildren(Geometry, ArrangedChildren);
	for (int32 Index = 0; Index < ArrangedChildren.Num(); ++Index)
	{
		TSharedPtr<SWidget> ChildWidget = ArrangedChildren[Index].Widget;
		FGeometry ChildGeometry = ArrangedChildren[Index].Geometry;
		
		//@todo: Don't understand why casting not working??? It's always return true .IsValid()
		//TSharedPtr<SViewport> Viewport = StaticCastSharedPtr<SViewport>(ChildWidget);
		// !!! OK !!! I know now why it is not working. We need dynamic cast. My Bad :{
		static FName NAME_Viewport(TEXT("SGameLayerManager"));
		if (ChildWidget->GetType() == NAME_Viewport)
		{
			OutGeometry = ArrangedChildren[Index].Geometry;
			return true;
		}
		else
		{
			const bool bResult = FindViewportGeometryInternal(ChildGeometry, ChildWidget, OutGeometry);
			if (bResult)
			{
				return true;
			}
		}
	}

	return false;
}

uint8 FRemoteControl::GetOrientation(int32 ControllerId)const
{
	FViewportInfo Info = GetViewportInfo(ControllerId);
	return Info.Size.X > Info.Size.Y ? 2 : 1;
}

bool FRemoteControl::Tick(float DeltaSeconds)
{
	const auto& Settings = *GetDefault< URemoteControlSettings >();
	
	//@todo: When starting game it's crashing if we are start the broadcast immediately. Jut wait for starting broadcast.
	class UGameViewportClient* const ViewportClient = GEngine->GameViewport;

	// Don't start the broadcast in the shipping build.
#if !UE_BUILD_SHIPPING
	if (Settings.bIsEnabled && ViewportClient && ViewportClient->GetWorld()->HasBegunPlay() && ViewportClient->GetWorld()->RealTimeSeconds > 0.25f && !bIsBroadcasting && !bPendingStart)
	{
		bPendingStart = true;
		StartTimeCounter = WAIT_TO_START_BROADCAST;
	}
	else if (!ViewportClient)
	{
		if (bIsBroadcasting)
		{
			StopBroadcast();
		}
		bPendingStart = false;
	}
	
	if (bPendingStart)
	{
		StartTimeCounter -= DeltaSeconds;
		if (StartTimeCounter <= 0)
		{
			bPendingStart = false;
			StartBroadcast();
		}
	}
#endif

	// Find viewport info of all players and create readback
	if (bPendingStart || bIsBroadcasting)
	{
		TArray<APlayerController*> PlayerControllers;
		GEngine->GetAllLocalPlayerControllers(PlayerControllers);
		for (int32 Index = 0; Index < PlayerControllers.Num(); Index++)
		{
			int32 ControllerId = Index;

			class APlayerController* const PC = PlayerControllers[Index];
			if (PC && PC->GetLocalPlayer())
			{
				FPlayerInfo Info;
				Info.ViewportClient = PC->GetLocalPlayer()->ViewportClient;
				Info.PlayerController = PC;

				FGeometry ViewportGeometry;
				const bool bResult = FindViewportGeometry(Info.ViewportClient->GetWindow(), ViewportGeometry);
				if (bResult)
				{
					Info.ViewportInfo.Position = ViewportGeometry.LocalToAbsolute(FVector2D::ZeroVector);
					Info.ViewportInfo.Size = ViewportGeometry.GetLocalSize();
				}
		
				PlayerInfos.Add(ControllerId, Info);

				// create readback info for player
				if (!PlayerReadbacks.Contains(ControllerId))
				{
					PlayerReadbacks.Add(ControllerId, MakeShareable(new FPlayerReadback(BroadcastResolution)));
				}
			}
		}
	}

	if (bIsBroadcasting && ViewportClient)
	{
		// Send ping/pong to clients every x seconds
		for (int32 Index = 0; Index < Communications.Num(); Index++)
		{
			Communications[Index]->Ping();
		}
		
		// Process received actions from device.
		FScopeLock Lock(&ReceivedActionsLock);
	
		for (int32 Index = 0; Index < ReceivedActions.Num(); Index++)
		{
			const FReceivedAction& ReceivedAction = ReceivedActions[Index];
			if (ReceivedAction.Action == REMOTE_ACTION_TOUCH)
			{
				OnTouchActionReceived(ReceivedAction);
			}
			else if (ReceivedAction.Action == REMOTE_ACTION_MOTION)
			{
				OnMotionActionReceived(ReceivedAction);
			}
		}

		ReceivedActions.Empty();
	}


	TArray<APlayerController*> PlayerControllers;
	GEngine->GetAllLocalPlayerControllers(PlayerControllers);
	if (PlayerControllers.Num() > 0 && PlayerControllers[0]->GetLocalPlayer())
	{
		UGameViewportClient* GameViewportClient = PlayerControllers[0]->GetLocalPlayer()->ViewportClient;
		FGeometry ViewportGeometry;
		const bool bResult = FindViewportGeometry_Bat(GameViewportClient->GetWindow(), ViewportGeometry);
		if (bResult)
		{
			BatPosition = ViewportGeometry.LocalToAbsolute(FVector2D::ZeroVector);
			BatSize = ViewportGeometry.GetLocalSize();
		}
	}

	return true;
}

bool FRemoteControl::FindViewportGeometry_Bat(TSharedPtr<SWindow> Window, FGeometry& OutGeometry)const
{
	if (Window.IsValid())
	{
		return FindViewportGeometryInternal_Bat(Window->GetWindowGeometryInWindow(), Window, OutGeometry);
	}

	return false;
}

bool FRemoteControl::FindViewportGeometryInternal_Bat(const FGeometry& Geometry, TSharedPtr<SWidget> Widget, FGeometry& OutGeometry)const
{
	FArrangedChildren ArrangedChildren(EVisibility::Visible);
	Widget->ArrangeChildren(Geometry, ArrangedChildren);
	for (int32 Index = 0; Index < ArrangedChildren.Num(); ++Index)
	{
		TSharedPtr<SWidget> ChildWidget = ArrangedChildren[Index].Widget;
		FGeometry ChildGeometry = ArrangedChildren[Index].Geometry;

		//@todo: Don't understand why casting not working??? It's always return true .IsValid()
		//TSharedPtr<SViewport> Viewport = StaticCastSharedPtr<SViewport>(ChildWidget);
		// !!! OK !!! I know now why it is not working. We need dynamic cast. My Bad :{
		static FName NAME_Viewport(TEXT("SGameLayerManager"));
		if (ChildWidget->GetType() == NAME_Viewport)
		{
			OutGeometry = ArrangedChildren[Index].Geometry;
			return true;
		}
		else
		{
			const bool bResult = FindViewportGeometryInternal_Bat(ChildGeometry, ChildWidget, OutGeometry);
			if (bResult)
			{
				return true;
			}
		}
	}

	return false;
}

void FRemoteControl::OnTouchActionReceived(const FReceivedAction& ReceivedAction)
{
	if (!PlayerInfos.Contains(ReceivedAction.ControllerId))
	{
		// Hmmmm, no player info found by given controller id
		return;
	}

	const auto& PlayerInfo = PlayerInfos[ReceivedAction.ControllerId];
	class UGameViewportClient* const ViewportClient = PlayerInfo.ViewportClient;

	uint8_t ControllerId = ReceivedAction.ControllerId;

	TArray<FTouchInfo> ParsedTouchs;
	if (ReceivedAction.Data.Num() > 0)
	{
		FBufferReader Buffer((void*)ReceivedAction.Data.GetData(), ReceivedAction.Data.Num(), false);
		
		while ((Buffer.TotalSize() - Buffer.Tell()) >= 9)
		{
			uint8 TouchId = 0;
			float TouchX = 0;
			float TouchY = 0;
			
			Buffer << TouchId;
			Buffer << TouchX;
			Buffer << TouchY;
			
			ParsedTouchs.Add(FTouchInfo(ControllerId, TouchId));

			FViewportInfo ViewportInfo = GetViewportInfo(ControllerId);

			const FVector2D TouchGame = FVector2D(TouchX, TouchY);

			if (HasTouchInfo(ControllerId, TouchId))
			{
				OnTouchMoved(ControllerId, TouchId, TouchGame);
				UpdateLastTouchInfo(ControllerId, TouchId, TouchGame);
			}
			else
			{
				OnTouchStarted(ControllerId, TouchId, TouchGame);
				LastTouchs.Add(FTouchInfo(ControllerId, TouchId, (TouchGame)));
			}

		}
	}

	// Detect ended touchs
	TArray<int32> Removed;
	for (int32 Index = 0; Index < LastTouchs.Num(); Index++)
	{
		const FTouchInfo& TouchInfo = LastTouchs[Index];
		int32 TouchId = TouchInfo.TouchId;


		bool bFound = false;
		for (int32 Index2 = 0; Index2 < ParsedTouchs.Num(); Index2++)
		{
			if (ParsedTouchs[Index2].ControllerId == ControllerId && ParsedTouchs[Index2].TouchId == TouchId)
			{
				bFound = true;
				break;
			}
		}

		if (TouchInfo.ControllerId == ControllerId && !bFound)
		{
			OnTouchEnded(ControllerId, TouchId, TouchInfo.Location);			
			Removed.Add(Index);
		}
	}

	for (int32 Index = 0; Index < Removed.Num(); Index++)
	{
		LastTouchs.RemoveAt(Removed[Index]);
	}
}

void FRemoteControl::OnMotionActionReceived(const FReceivedAction& ReceivedAction)
{
	if (PlayerInfos.Contains(ReceivedAction.ControllerId))
	{
		const auto& PlayerInfo = PlayerInfos[ReceivedAction.ControllerId];
		if (PlayerInfo.PlayerController && ReceivedAction.Data.Num() >= 48)
		{
			FBufferReader Buffer((void*)ReceivedAction.Data.GetData(), ReceivedAction.Data.Num(), false);

			FVector Tilt = FVector::ZeroVector;
			FVector RotationRate = FVector::ZeroVector;
			FVector Gravity = FVector::ZeroVector;
			FVector Acceleration = FVector::ZeroVector;
			FRotator HeadTransform = FRotator::ZeroRotator;

			Buffer << Tilt;
			Buffer << RotationRate;
			Buffer << Gravity;
			Buffer << Acceleration;

			// Update v2.0 - Get HeadTransform only if buffer size greater then 48. Otherwise will be crash...
			if (ReceivedAction.Data.Num() >= 51)
			{
				Buffer << HeadTransform;
			}

			{
				FScopeLock Lock(&LastHreadTransformCS);
				LastHeadTransform = HeadTransform;
			}
			
			PlayerInfo.PlayerController->InputMotion(Tilt, RotationRate, Gravity, Acceleration);

			FMotionEvent MotionEvent(ReceivedAction.ControllerId, Tilt, RotationRate, Gravity, Acceleration);
			FSlateApplication::Get().ProcessMotionDetectedEvent(MotionEvent);
		}
	}
}

FRotator FRemoteControl::GetHeadTransform()
{
	FScopeLock Lock(&LastHreadTransformCS);
	return LastHeadTransform;
}

void FRemoteControl::ResetHeadTracker()
{
	//@todo: Multiplayer VR not supporting!
	//@todo: This is little important. Send it three times. No one hurts.
	for (int32 Idx = 0; Idx < 3; Idx++)
	{
		SendAction(0, REMOTE_ACTION_RESET_HEAD_TRACKER);
	}
}

FVector2D FRemoteControl::ConvertTouchToScreen(int32 ControllerID, const FVector2D TouchPosition)
{
	if (PlayerInfos.Contains(ControllerID))
	{
		const auto& PlayerInfo = PlayerInfos[ControllerID];
		FVector2D ScreenPosition;
		
		FSlateApplication& SlateApplication = FSlateApplication::Get();

		// The remote is interested in finding the game viewport for the user and mapping the input in to it
		TSharedPtr<SViewport> GameViewport = PlayerInfo.ViewportClient->GetGameViewportWidget();
		if (GameViewport.IsValid())
		{
			FWidgetPath WidgetPath;

			FWeakWidgetPath GameViewportWidgetPath;

			WidgetPath = GameViewportWidgetPath.ToWidgetPath();
			SlateApplication.FindPathToWidget(GameViewport.ToSharedRef(), WidgetPath);
			GameViewportWidgetPath = WidgetPath;

			const FGeometry& GameViewportGeometry = WidgetPath.Widgets.Last().Geometry;
			ScreenPosition = GameViewportGeometry.LocalToAbsolute(TouchPosition * GameViewportGeometry.Size);
		}
		else
		{
			const FSlateRect WindowScreenRect = SlateApplication.GetActiveTopLevelWindow()->GetRectInScreen();
			const FVector2D WindowPosition = WindowScreenRect.GetSize() * TouchPosition;
			ScreenPosition = FVector2D(WindowScreenRect.Left, WindowScreenRect.Top) + WindowPosition;
		}

		return ScreenPosition;
	}

	return FVector2D::ZeroVector;
}

bool FRemoteControl::HasTouchInfo(int32 ControllerID, int32 TouchID)const
{
	for (const FTouchInfo& TouchInfo : LastTouchs)
	{
		if (TouchInfo.ControllerId == ControllerID && TouchInfo.TouchId == TouchID)
		{
			return true;
		}
	}

	return false;
}

bool FRemoteControl::GetLastTouchInfo(int32 ControllerID, int32 TouchID, FTouchInfo& OutTouchInfo)const
{
	for (const FTouchInfo& TouchInfo : LastTouchs)
	{
		if (TouchInfo.ControllerId == ControllerID && TouchInfo.TouchId == TouchID)
		{
			OutTouchInfo = TouchInfo;
			return true;
		}
	}

	return false;
}

void FRemoteControl::UpdateLastTouchInfo(int32 ControllerID, int32 TouchID, FVector2D TouchLocation)
{
	for (FTouchInfo& TouchInfo : LastTouchs)
	{
		if (TouchInfo.ControllerId == ControllerID && TouchInfo.TouchId == TouchID)
		{
			TouchInfo.Location = TouchLocation;
			break;
		}
	}
}

void FRemoteControl::OnTouchStarted(int32 ControllerId, int32 TouchId, FVector2D TouchLocation)
{
	if (PlayerInfos.Contains(ControllerId))
	{
		auto PlayerInfo = PlayerInfos[ControllerId];
		
		const FVector2D TouchScreen = ConvertTouchToScreen(ControllerId, TouchLocation);

		FPointerEvent PointerEvent(
			ControllerId,
			TouchId,
			TouchScreen,
			TouchScreen,
			true);

		if (!FSlateApplication::Get().ProcessMouseButtonDownEvent(PlayerInfo.ViewportClient->GetWindow()->GetNativeWindow(), PointerEvent))
		{
			// Touch not handled by slate. redirect to PlayerController.
			if (PlayerInfo.PlayerController)
			{
				PlayerInfo.PlayerController->InputTouch(TouchId, ETouchType::Began, TouchLocation, FDateTime::Now(), 0);
			}
		}
		
		//@fixme:	There is a bug between drag&drop and touch. You cannot drag with touch! This is workaround for it.
		//			But this will not work with multiplayer.
		if (TouchId == 0 && PlayerInfos.Num() == 0 && FSlateApplication::Get().IsDragDropping())
		{
			FSlateApplication::Get().SetCursorPos(TouchScreen);
		}
	}
}

void FRemoteControl::OnTouchMoved(int32 ControllerId, int32 TouchId, FVector2D TouchLocation)
{
	if (PlayerInfos.Contains(ControllerId))
	{
		auto PlayerInfo = PlayerInfos[ControllerId];

		const FVector2D TouchScreen = ConvertTouchToScreen(ControllerId, TouchLocation);

		FVector2D LastLocation = TouchScreen;
		
		FTouchInfo LastTouchInfo;
		const bool bResult = GetLastTouchInfo(ControllerId, TouchId, LastTouchInfo);
		if (bResult)
		{
			LastLocation = ConvertTouchToScreen(ControllerId, LastTouchInfo.Location);
		}

		FPointerEvent PointerEvent(
			ControllerId,
			TouchId,
			TouchScreen,
			LastLocation,
			true);
		
		if (!FSlateApplication::Get().ProcessMouseMoveEvent(PointerEvent))
		{
			// Touch not handled by slate. redirect to PlayerController.
			if (PlayerInfo.PlayerController)
			{
				PlayerInfo.PlayerController->InputTouch(TouchId, ETouchType::Moved, TouchLocation, FDateTime::Now(), 0);
			}
		}

		//@fixme:	There is a bug between drag&drop and touch. You cannot drag with touch! This is workaround for it.
		//			But this will not work with multiplayer.
		if (TouchId == 0 && PlayerInfos.Num() == 0 && FSlateApplication::Get().IsDragDropping())
		{
			FSlateApplication::Get().SetCursorPos(TouchScreen);
		}
	}
}

void FRemoteControl::OnTouchEnded(int32 ControllerId, int32 TouchId, FVector2D TouchLocation)
{
	if (PlayerInfos.Contains(ControllerId))
	{
		auto PlayerInfo = PlayerInfos[ControllerId];

		const FVector2D TouchScreen = ConvertTouchToScreen(ControllerId, TouchLocation);

		FPointerEvent PointerEvent(
			ControllerId,
			TouchId,
			TouchScreen,
			TouchScreen,
			true);

		if (!FSlateApplication::Get().ProcessMouseButtonUpEvent(PointerEvent))
		{
			// Touch not handled by slate. redirect to PlayerController.
			if (PlayerInfo.PlayerController)
			{
				PlayerInfo.PlayerController->InputTouch(TouchId, ETouchType::Ended, TouchLocation, FDateTime::Now(), 0);
			}
		}
	}
}

void FRemoteControl::StartBroadcast()
{
	// Provide empty param so settings will be used.
	StartBroadcastInternal(TEXT(""));
}

void FRemoteControl::StopBroadcast()
{
	if (bIsBroadcasting)
	{
		bIsBroadcasting = false;
		
		// Stop all communications
		for (int32 Index = 0; Index < Communications.Num(); Index++)
		{
			Communications[Index]->Stop();
		}
		Communications.Empty();

		// Stop UDP
		StopUDP();
		
		if (FSlateApplication::IsInitialized())	// During shutdown, Slate may have already been destroyed by the time our viewport gets cleaned up
		{
			FSlateRenderer* SlateRenderer = FSlateApplication::Get().GetRenderer();//.Get();
			SlateRenderer->OnSlateWindowRendered().RemoveAll(this);
		}

		// Cleanup readback buffer textures
		{
			FlushRenderingCommands();

			for (auto It = PlayerReadbacks.CreateConstIterator(); It; ++It)
			{
				auto PlayerReadback = It.Value();
				PlayerReadback->ReleaseResources();
			}

			PlayerReadbacks.Empty();
		}

		// Close ADB connection
		CloseADBConnection();
	}
}

void FRemoteControl::StartBroadcastWithCustomIp(FString Ip)
{
	StartBroadcastInternal(Ip);
}

void FRemoteControl::StartBroadcastInternal(FString Ip)
{
	if (!bIsBroadcasting)
	{
		// Clear previous player infos
		PlayerInfos.Empty();

		// Clear previous player readbacks
		PlayerReadbacks.Empty();
		
		// Open ADB connection
		OpenADBConnection();

		// Initialize communication
		InitCommunication(Ip);
				
		// Start UDP
		StartUDP();

		UGameViewportClient* GameViewportClient = GEngine->GameViewport;
		check(GameViewportClient != nullptr);

		const auto& Settings = *GetDefault< URemoteControlSettings >();
		
		BroadcastResolution = FVector2D(256.f, 256.f);
		switch (Settings.Resolution)
		{
		case ERemoteResolution::r128:
			BroadcastResolution = FVector2D(128, 128);
			break;
		case ERemoteResolution::r256:
			BroadcastResolution = FVector2D(256, 256);
			break;
		case ERemoteResolution::r512:
			BroadcastResolution = FVector2D(512, 512);
			break;
		case ERemoteResolution::r768:
			BroadcastResolution = FVector2D(768, 768);
			break;
		case ERemoteResolution::r1024:
			BroadcastResolution = FVector2D(1024, 1024);
			break;
		}

		// Register slate rendered delegate
		FSlateRenderer* SlateRenderer = FSlateApplication::Get().GetRenderer();//.Get();
		SlateRenderer->OnSlateWindowRendered().RemoveAll(this);
		SlateRenderer->OnSlateWindowRendered().AddRaw(this, &FRemoteControl::OnSlateRendered);
		
		bIsBroadcasting = true;
	}
}

void FRemoteControl::OnSlateRendered(SWindow& SlateWindow, void* ViewportRHIPtr)
{

	UGameViewportClient* GameViewportClient = GEngine->GameViewport;
	if (GameViewportClient)
	{
		const FViewportRHIRef* ViewportRHI = (const FViewportRHIRef*)ViewportRHIPtr;
		StartCopyingNextVideoFrame(0, *ViewportRHI);
	}


	//const auto& Settings = *GetDefault< URemoteControlSettings >();

	//float Frame = 1.f / (float)Settings.FrameRate;
	//double CurrentTime = FApp::GetCurrentTime();


	//UGameViewportClient* GameViewportClient = GEngine->GameViewport;
	//if (bIsBroadcasting && GameViewportClient)
	//{
	//	for (auto It = PlayerInfos.CreateConstIterator(); It; ++It)
	//	{
	//		int32 ControllerId = It.Key();
	//		auto PlayerInfo = It.Value();

	//		if (PlayerInfo.ViewportClient->GetWindow() == SlateWindow.AsShared())
	//		{
	//			// Check to see if there are any video frames ready to push
	//			BroadcastVideoFrame(ControllerId);

	//			float Diff = CurrentTime - (LastBroadcastTimes.Contains(ControllerId) ? LastBroadcastTimes[ControllerId] : 0.f);
	//			if (Diff >= Frame)
	//			{
	//				LastBroadcastTimes.Add(ControllerId, CurrentTime);
	//				
	//				// Start copying next game video frame
	//				const FViewportRHIRef* ViewportRHI = (const FViewportRHIRef*)ViewportRHIPtr;
	//				StartCopyingNextVideoFrame(ControllerId, *ViewportRHI);
	//			}

	//		}
	//	}
	//}
	//else
	//{
	//	// No longer broadcasting.
	//	StopBroadcast();
	//}
}

void FRemoteControl::BroadcastVideoFrame(int32 ControllerId)
{
	auto PlayerReadback = PlayerReadbacks[ControllerId];
	if (PlayerReadback.IsValid())
	{
		if (PlayerReadback->IsBufferReady())
		{
			SendVideoFrameBuffer(ControllerId, (uint8*)PlayerReadback->GetBuffer(), PlayerReadback->GetBufferSize(), PlayerReadback->Resolution.X, PlayerReadback->Resolution.Y);
			PlayerReadback->Unmap();
		}
		else
		{
			// Frame hasn't finished copying from the GPU, so we can't broadcast a frame yet
		}
	}
}

void FRemoteControl::StartCopyingNextVideoFrame(int32 ControllerId, const FViewportRHIRef& ViewportRHI)
{
	static const FName RendererModuleName("Renderer");
	IRendererModule& RendererModule = FModuleManager::GetModuleChecked<IRendererModule>(RendererModuleName);

	UGameViewportClient* GameViewportClient = GEngine->GameViewport;
	check(GameViewportClient != nullptr);


		FVector2D WindowSize = GameViewportClient->GetWindow()->GetSizeInScreen();
		FVector2D UV = BatPosition / WindowSize;
		FVector2D UVSize = BatSize / WindowSize;

		struct FCopyVideoFrame
		{
			FViewportRHIRef ViewportRHI;
			IRendererModule* RendererModule;
			FIntPoint Resolution;
			FVector2D UV;
			FVector2D UVSize;
			//TSharedPtr<FPlayerReadback, ESPMode::ThreadSafe> PlayerReadback;
			//FRemoteControl* This;
		};
		FCopyVideoFrame CopyVideoFrame =
		{
			ViewportRHI,
			&RendererModule,
			FIntPoint(BroadcastResolution.X, BroadcastResolution.Y),
			UV,
			UVSize,
			//PlayerReadback,
			//this
		};
		
		FCopyVideoFrame Context = CopyVideoFrame;
		ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)(
			[Context](FRHICommandListImmediate& RHICmdList)
		{
				FPooledRenderTargetDesc OutputDesc(FPooledRenderTargetDesc::Create2DDesc(Context.Resolution, PF_R8G8B8A8, FClearValueBinding::None, TexCreate_None, TexCreate_RenderTargetable, false));

				const auto FeatureLevel = GMaxRHIFeatureLevel;

				TRefCountPtr<IPooledRenderTarget> ResampleTexturePooledRenderTarget;
				Context.RendererModule->RenderTargetPoolFindFreeElement(RHICmdList, OutputDesc, ResampleTexturePooledRenderTarget, TEXT("RemoteControlTexture"));
				check(ResampleTexturePooledRenderTarget);

				const FSceneRenderTargetItem& DestRenderTarget = ResampleTexturePooledRenderTarget->GetRenderTargetItem();

				SetRenderTarget(RHICmdList, DestRenderTarget.TargetableTexture, FTextureRHIRef());
				RHICmdList.SetViewport(0, 0, 0.0f, Context.Resolution.X, Context.Resolution.Y, 1.0f);

					FGraphicsPipelineStateInitializer GraphicsPSOInit;
					RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

					GraphicsPSOInit.BlendState = TStaticBlendState<CW_RGB>::GetRHI();
					GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
					GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false,CF_Always>::GetRHI();

				// @todo livestream: Ideally this "desktop background color" should be configurable in the editor's preferences
				//RHICmdList.Clear(true, FLinearColor(0.02f, 0.02f, 0.2f), false, 0.f, false, 0x00, FIntRect());

				FTexture2DRHIRef ViewportBackBuffer = RHICmdList.GetViewportBackBuffer(Context.ViewportRHI);
				
				auto ShaderMap = GetGlobalShaderMap(FeatureLevel);
				TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
				TShaderMapRef<FScreenPS> PixelShader(ShaderMap);

				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = Context.RendererModule->GetFilterVertexDeclaration().VertexDeclarationRHI;
				GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
				GraphicsPSOInit.PrimitiveType = PT_TriangleList;

				//SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
				RHICmdList.SetGraphicsPipelineState(GetAndOrCreateGraphicsPipelineState(RHICmdList, GraphicsPSOInit, EApplyRendertargetOption::CheckApply));

				if (Context.Resolution != FIntPoint(ViewportBackBuffer->GetSizeX(), ViewportBackBuffer->GetSizeY()))
				{
					// We're scaling down the window, so use bilinear filtering
					PixelShader->SetParameters(RHICmdList, TStaticSamplerState<SF_Bilinear>::GetRHI(), ViewportBackBuffer);
				}
				else
				{
					// Drawing 1:1, so no filtering needed
					PixelShader->SetParameters(RHICmdList, TStaticSamplerState<SF_Point>::GetRHI(), ViewportBackBuffer);
				}

				Context.RendererModule->DrawRectangle(
					RHICmdList,
					0, 0,		// Dest X, Y
					Context.Resolution.X, Context.Resolution.Y,	// Dest Width, Height
					Context.UV.X, Context.UV.Y,		// Source U, V
					Context.UVSize.X, Context.UVSize.Y,		// Source USize, VSize
					Context.Resolution,		// Target buffer size
					FIntPoint(1, 1),		// Source texture size
					*VertexShader,
					EDRF_Default);

				FIntRect Rect = FIntRect(0, 0, Context.Resolution.X, Context.Resolution.Y);

				TArray<FColor> OutData ;
				RHICmdList.ReadSurfaceData(
					DestRenderTarget.TargetableTexture,
					Rect,
					OutData,
					FReadSurfaceDataFlags()
					);

				// BGRA to RGBA
				for (int32 Index = 0; Index < OutData.Num(); Index++)
				{
					auto Tmp = OutData[Index].B;
					OutData[Index].B = OutData[Index].R;
					OutData[Index].R = Tmp;
				}

				//UE_LOG(LogTemp, Warning, TEXT("2RemoteControl OutData.Num() %s"), *OutData[0].ToString());
				//UE_LOG(LogTemp, Warning, TEXT("2RemoteControl OutData.Num() %s"), *OutData[10000].ToString());
						
				//FMemory::Memcpy(Context.PlayerReadback->GetBuffer(), OutData.GetData(), Context.PlayerReadback->GetBufferSize());
				//Context.PlayerReadback->SetBufferReady(true);
			});
}

#undef LOCTEXT_NAMESPACE
