// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FrameExperimentsPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"


#include "SlateBasics.h"
#include "ScreenRendering.h"
#include "RenderCore.h"
#include "RHIStaticStates.h"
#include "ISettingsModule.h"
#include "RendererInterface.h"
#include "SlateApplication.h"
#include "PipelineStateCache.h"

#include "Engine.h"

#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"



AFrameExperimentsPawn::AFrameExperimentsPawn()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());	// Set static mesh
	RootComponent = PlaneMesh;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);	// Attach SpringArm to RootComponent
	SpringArm->TargetArmLength = 160.0f; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f,0.f,60.f);
	SpringArm->bEnableCameraLag = false;	// Do not allow camera to lag
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	// Attach the camera
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 500.f;
	TurnSpeed = 50.f;
	MaxSpeed = 4000.f;
	MinSpeed = 500.f;
	CurrentForwardSpeed = 500.f;
}

void AFrameExperimentsPawn::BeginPlay()
{
	Super::BeginPlay();

	MyTex = UTexture2D::CreateTransient(800, 600, PF_B8G8R8A8);

	// Setup texture dynamicly
	DynamicMatInstance = PlaneMesh->CreateAndSetMaterialInstanceDynamic(0);
	if (DynamicMatInstance)
	{
		//DynamicMatInstance->SetTextureParameterValue(FName("BatTex"), MyTex);
	}

	// Read Player Controller
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UE_LOG(LogTemp, Warning, TEXT("PlayerController %p"), PlayerController);

	// Initialize buffer
	ReadbackBuffer = new uint8[800 * 600 * 4];

	// Register slate rendered delegate
	FSlateRenderer* SlateRenderer = FSlateApplication::Get().GetRenderer();//.Get();
	SlateRenderer->OnSlateWindowRendered().AddUObject(this, &AFrameExperimentsPawn::OnSlateRendered);
}

void AFrameExperimentsPawn::OnSlateRendered(SWindow& SlateWindow, void* ViewportRHIPtr)
{
	const FViewportRHIRef* ViewportRHI = (const FViewportRHIRef*)ViewportRHIPtr;
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
	};

	FCopyVideoFrame CopyVideoFrame =
	{
		*ViewportRHI,
		&RendererModule,
		FIntPoint(800, 600),
		UV,
		UVSize,
	};

	FCopyVideoFrame Context = CopyVideoFrame;

	ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)(
		[&, Context](FRHICommandListImmediate& RHICmdList)
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
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();

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

		// Drawing 1:1, so no filtering needed
		PixelShader->SetParameters(RHICmdList, TStaticSamplerState<SF_Point>::GetRHI(), ViewportBackBuffer);

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

		TArray<FColor> OutData;
		RHICmdList.ReadSurfaceData(
			DestRenderTarget.TargetableTexture,
			Rect,
			OutData,
			FReadSurfaceDataFlags()
		);

		// Test update texture
		//UE_LOG(LogTemp, Warning, TEXT("ViewportBackBuffer->GetSizeX() %d"), ViewportBackBuffer->GetSizeX());
		//UE_LOG(LogTemp, Warning, TEXT("ViewportBackBuffer->GetSizeY() %d"), ViewportBackBuffer->GetSizeY());

		//UE_LOG(LogTemp, Warning, TEXT("in render callback Rect %s"), *Rect.ToString());
		UE_LOG(LogTemp, Warning, TEXT("in render callback OutData.Num() 0 %s"), *OutData[0].ToString());
		UE_LOG(LogTemp, Warning, TEXT("in render callback OutData.Num() 100 %s"), *OutData[100].ToString());
		UE_LOG(LogTemp, Warning, TEXT("in render callback OutData.Num() 10000 %s"), *OutData[10000].ToString());


		// BGRA to RGBA
		for (int32 Index = 0; Index < OutData.Num(); Index++)
		{
			auto Tmp = OutData[Index].B;
			OutData[Index].B = OutData[Index].R;
			OutData[Index].R = Tmp;
		}

		FMemory::Memcpy(ReadbackBuffer, OutData.GetData(), 800 * 600 * 4);
		bIsBufferReady = true;
	});

}

void AFrameExperimentsPawn::Tick(float DeltaSeconds)
{
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0,0,0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);


	// Update Viewport geometry
	if (PlayerController && PlayerController->GetLocalPlayer())
	{
		UGameViewportClient* GameViewportClient = PlayerController->GetLocalPlayer()->ViewportClient;
		FGeometry ViewportGeometry;
		const bool bResult = FindViewportGeometry_Bat(GameViewportClient->GetWindow(), ViewportGeometry);
		if (bResult)
		{
			BatPosition = ViewportGeometry.LocalToAbsolute(FVector2D::ZeroVector);
			BatSize = ViewportGeometry.GetLocalSize();
		}
	}


	// Update texture
	if (bIsBufferReady && MyTex)
	{
		UE_LOG(LogTemp, Warning, TEXT("bIsBufferReady && MyTex"));


		void* TextureData = MyTex->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, ReadbackBuffer, 800 * 600 * 4);
		MyTex->PlatformData->Mips[0].BulkData.Unlock();
		MyTex->UpdateResource();
	}
}


bool AFrameExperimentsPawn::FindViewportGeometry_Bat(TSharedPtr<SWindow> Window, FGeometry& OutGeometry)const
{
	if (Window.IsValid())
	{
		return FindViewportGeometryInternal_Bat(Window->GetWindowGeometryInWindow(), Window, OutGeometry);
	}

	return false;
}

bool AFrameExperimentsPawn::FindViewportGeometryInternal_Bat(const FGeometry& Geometry, TSharedPtr<SWidget> Widget, FGeometry& OutGeometry)const
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


void AFrameExperimentsPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Deflect along the surface when we collide.
	FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
}


void AFrameExperimentsPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Check if PlayerInputComponent is valid (not NULL)
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("Thrust", this, &AFrameExperimentsPawn::ThrustInput);
	PlayerInputComponent->BindAxis("MoveUp", this, &AFrameExperimentsPawn::MoveUpInput);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFrameExperimentsPawn::MoveRightInput);
}

void AFrameExperimentsPawn::ThrustInput(float Val)
{
	// Is there any input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void AFrameExperimentsPawn::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AFrameExperimentsPawn::MoveRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value.
	float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}


void AFrameExperimentsPawn::BeginDestroy()
{
	Super::BeginDestroy();

	delete[](uint8*)ReadbackBuffer;
	ReadbackBuffer = nullptr;
}