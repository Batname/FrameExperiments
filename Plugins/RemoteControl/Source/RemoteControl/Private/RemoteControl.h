#pragma once

#include "Networking.h"
#include "IRemoteControl.h"
#include "RemoteActions.h"

class FRemoteUDPReceiver;
class FRemoteUDPSender;

class FRemoteControl
	: public IRemoteControl
{

public:
	/** Received action info from device. */
	struct FReceivedAction
	{
		int32 ControllerId;
		int32 Action;
		TArray<uint8> Data;
		
		FReceivedAction(int32 InControllerId, int32 InAction, const TArray<uint8> InData)
			: ControllerId(InControllerId)
			, Action(InAction)
			, Data(InData)
		{

		}
	};

	/** Touch info from device */
	struct FTouchInfo
	{
		int32 ControllerId;
		int32 TouchId;
		FVector2D Location;

		FTouchInfo()
			: ControllerId(0)
			, TouchId(0)
			, Location(FVector::ZeroVector)
		{

		}

		FTouchInfo(int32 InControllerId, int32 InTouchId)
			: ControllerId(InControllerId)
			, TouchId(InTouchId)
			, Location(FVector::ZeroVector)
		{

		}

		FTouchInfo(int32 InControllerId, int32 InTouchId, const FVector2D& InLocation)
			: ControllerId(InControllerId)
			, TouchId(InTouchId)
			, Location(InLocation)
		{

		}
	};

	/** Viewport Info */
	struct FViewportInfo
	{
		FVector2D Position;
		FVector2D Size;

		FViewportInfo()
			: Position(FVector2D::ZeroVector)
			, Size(FVector2D::ZeroVector)
		{

		}
	};

	/** Player info */
	struct FPlayerInfo
	{
		UGameViewportClient* ViewportClient;
		APlayerController* PlayerController;
		FViewportInfo ViewportInfo;

		FPlayerInfo()
			: ViewportClient(nullptr)
			, PlayerController(nullptr)
		{

		}
	};
	
	TMap<int32, FPlayerInfo> PlayerInfos;

	/** Player readback */
	struct FPlayerReadback
	{
		FIntPoint Resolution;
		uint32 BufferSize;

		void* ReadbackBuffer;

		FPlayerReadback(FVector2D InResolution)
			: Resolution(FIntPoint(InResolution.X, InResolution.Y))
			, bIsBufferReady(false)
		{
			BufferSize = Resolution.X * Resolution.Y * 4;

			// Initialize buffer
			ReadbackBuffer = new uint8[BufferSize];
		}
		
		void ReleaseResources()
		{
			delete[] (uint8*)ReadbackBuffer;
			ReadbackBuffer = nullptr;
		}

		FORCEINLINE bool IsBufferReady(){return bIsBufferReady;}

		void SetBufferReady(bool bValue){bIsBufferReady = bValue;}

		FORCEINLINE uint32 GetBufferSize()const {return BufferSize;}

		void*& GetBuffer(){return ReadbackBuffer;}

		void Unmap(){bIsBufferReady = false;}

		private:
			bool bIsBufferReady;
	};
	TMap<int32, TSharedPtr<FPlayerReadback, ESPMode::ThreadSafe>> PlayerReadbacks;
	
	TMap<int32, double> LastBroadcastTimes;
public:

	/** Default constructor */
	FRemoteControl();

	/** IModuleInterface overrides */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void StartUDP();
	void StopUDP();

	virtual bool IsBroadcasting() const { return bIsBroadcasting; }
	virtual void StartBroadcast();
	virtual void StopBroadcast();
	virtual void StartBroadcastWithCustomIp(FString Ip);
	virtual FRotator GetHeadTransform() override;
	virtual void ResetHeadTracker() override;

	void StartBroadcastInternal(FString Ip = TEXT(""));


	bool Tick(float DeltaSeconds);

	void OnTouchActionReceived(const FReceivedAction& ReceivedAction);
	void OnMotionActionReceived(const FReceivedAction& ReceivedAction);
	
	bool FindViewportGeometry(TSharedPtr<SWindow> Window, FGeometry& OutGeometry)const;
	bool FindViewportGeometryInternal(const FGeometry& Geometry, TSharedPtr<SWidget> Widget, FGeometry& OutGeometry)const;
	class FRemoteCommunication* GetCommunication(int32 ControllerId) const;
	uint8 GetOrientation(int32 ControllerId = 0) const;
	
	FORCEINLINE FRemoteUDPSender* GetUDPSender()const { return UDPSender; }

	FViewportInfo GetViewportInfo(int32 ControllerId = 0)const;

	void SendAction(int32 ControllerId, uint32 InAction);
	void SendAction(int32 ControllerId, uint32 InAction, const FBufferArchive& InData);

	virtual void OnControllerChanged(FRemoteCommunication* Comm, int32 OldControllerId, int32 NewControllerId);

	void SyncActions();

	virtual void OnActionReceived(int32 ControllerId, int32 Action, const TArray<uint8> Data);
	
	/**
	Initialize communication between devices.
	If "Ip" param specified other settings will be ignored.
	*/
	void InitCommunication(FString Ip = TEXT(""));

	void OnUDPReceived(const FArrayReaderPtr& Data, const FIPv4Endpoint& Addr);

	/** Get ADB path */
	FString GetADBPath()const;

	/** Open ADB connection for android devices */
	void OpenADBConnection();
	
	/** Close ADB connection for android devices */
	void CloseADBConnection();

	/**
	 * Called by the Slate rendered on the game thread right after a window has been rendered, to allow us to
	 * capture frame buffer content after everything has been rendered, including debug canvas graphics
	 *
	 * @param	SlateWindow				The window that was just rendered
	 * @param	ViewportRHIPtr			The viewport we rendered this window to (cast to FViewportRHIRef*)
	 */
	void OnSlateRendered( SWindow& SlateWindow, void* ViewportRHIPtr );

	/** Broadcasts a new video frame, only if the frame's data has finished copying from the GPU to system memory */
	void BroadcastVideoFrame(int32 ControllerId);

	/** Starts copying the last rendered game frame from the GPU back buffer to a mapped system memory texture */
	void StartCopyingNextVideoFrame(int32  ControllerId, const FViewportRHIRef& ViewportRHI);

	void SendVideoFrameBuffer(int32 ControllerId, const uint8* VideoFrameBuffer, uint32 BufferSize, int32 Width, int32 Height);

	/** Touch position range is [0..1] */
	FVector2D ConvertTouchToScreen(int32 ControllerID, const FVector2D TouchPosition);

	bool HasTouchInfo(int32 ControllerID, int32 TouchID)const;
	bool GetLastTouchInfo(int32 ControllerID, int32 TouchID, FTouchInfo& OutTouchInfo)const;
	void UpdateLastTouchInfo(int32 ControllerID, int32 TouchID, FVector2D TouchLocation);

	void OnTouchStarted(int32 ControllerId, int32 TouchId, FVector2D TouchLocation);
	void OnTouchMoved(int32 ControllerId, int32 TouchId, FVector2D TouchLocation);
	void OnTouchEnded(int32 ControllerId, int32 TouchId, FVector2D TouchLocation);
private:
	/** Delegate for callbacks to Tick */
	FTickerDelegate TickDelegate;

	/** Handle to various registered delegates */
	FDelegateHandle TickDelegateHandle;
	
	FSocket* SocketUdp;
	FRemoteUDPReceiver* UDPReceiver;
	FRemoteUDPSender* UDPSender;

	FVector2D BroadcastResolution;
	bool bPendingStart;
	float StartTimeCounter;
	
	TArray<FRemoteCommunication*> Communications;

	TArray<FAsyncTask<FRemoteActionImageCompress>*> Actions;
	FCriticalSection SyncActionsSection;
	
	TArray<FReceivedAction> ReceivedActions;
	FCriticalSection ReceivedActionsLock;

	TArray<FTouchInfo> LastTouchs;

	bool bIsBroadcasting;

	/** Last head tracker transform */
	FCriticalSection LastHreadTransformCS;

	/** Last head tracker transform */
	FRotator LastHeadTransform;

private:
	FVector2D BatPosition = FVector2D::ZeroVector;
	FVector2D BatSize = FVector2D::ZeroVector;
	bool FindViewportGeometry_Bat(TSharedPtr<SWindow> WindowWidget, FGeometry& OutGeometry)const;
	bool FindViewportGeometryInternal_Bat(const FGeometry& Geometry, TSharedPtr<SWidget> Widget, FGeometry& OutGeometry)const;

};
