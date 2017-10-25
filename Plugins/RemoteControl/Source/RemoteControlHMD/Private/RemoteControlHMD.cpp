#include "RemoteControlHMDPrivatePCH.h"
#include "RemoteControlHMD.h"

#include "Runtime/Renderer/Private/RendererPrivate.h"
#include "Runtime/Renderer/Private/ScenePrivate.h"
#include "Runtime/Renderer/Private/PostProcess/PostProcessHMD.h"

#include "EngineAnalytics.h"
#include "Runtime/Analytics/Analytics/Public/Interfaces/IAnalyticsProvider.h"
#include "HardwareInfo.h"

#include "IRemoteControl.h"
#include "RemoteControlSettings.h"

#include "SceneViewport.h"

FRemoteControlHMDModule::FRemoteControlHMDModule()
{
	bInitialized = false;
	bInitializeCalled = false;
}

bool FRemoteControlHMDModule::Initialize()
{
	if(!bInitializeCalled)
	{
		bInitialized = false;
		bInitializeCalled = true;

		bInitialized = true;
	}

	return bInitialized;
}

void FRemoteControlHMDModule::ShutdownModule()
{
	bInitialized = false;
	bInitializeCalled = false;
}

FString FRemoteControlHMDModule::GetModuleKeyName() const
{
	return FString(TEXT("RemoteControlHMD"));
}

bool FRemoteControlHMDModule::PreInit()
{
	return true;
}

bool FRemoteControlHMDModule::IsHMDConnected()
{
	return true;
}

int FRemoteControlHMDModule::GetGraphicsAdapter()
{
	return -1;
}

FString FRemoteControlHMDModule::GetAudioInputDevice()
{
	return TEXT("");
}

FString FRemoteControlHMDModule::GetAudioOutputDevice()
{
	return TEXT("");
}

TSharedPtr< class IHeadMountedDisplay, ESPMode::ThreadSafe > FRemoteControlHMDModule::CreateHeadMountedDisplay()
{
	if (Initialize())
	{
		TSharedPtr< FRemoteControlHMD, ESPMode::ThreadSafe > HMD(new FRemoteControlHMD());
		if (HMD->IsInitialized())
		{
			HeadMountedDisplay = HMD;
			return HMD;
		}
	}
	HeadMountedDisplay = nullptr;
	return NULL;
}

IMPLEMENT_MODULE( FRemoteControlHMDModule, RemoteControlHMD )

///////////////////////////////////////////////////////////////

//---------------------------------------------------
// SimpleHMD IHeadMountedDisplay Implementation
//---------------------------------------------------

bool FRemoteControlHMD::IsHMDEnabled() const
{
	return true;
}

void FRemoteControlHMD::EnableHMD(bool enable)
{
}

EHMDDeviceType::Type FRemoteControlHMD::GetHMDDeviceType() const
{
	return EHMDDeviceType::DT_ES2GenericStereoMesh;
}

bool FRemoteControlHMD::GetHMDMonitorInfo(MonitorInfo& MonitorDesc)
{
	MonitorDesc.MonitorName = "";
	MonitorDesc.MonitorId = 0;
	MonitorDesc.DesktopX = MonitorDesc.DesktopY = MonitorDesc.ResolutionX = MonitorDesc.ResolutionY = 0;
	return false;
}

void FRemoteControlHMD::GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const
{
	OutHFOVInDegrees = 0.0f;
	OutVFOVInDegrees = 0.0f;
}

void FRemoteControlHMD::GetPositionalTrackingCameraProperties(FVector& OutOrigin, FQuat& OutOrientation, float& OutHFOV, float& OutVFOV, float& OutCameraDistance, float& OutNearPlane, float& OutFarPlane) const
{
}

void FRemoteControlHMD::RebaseObjectOrientationAndPosition(FVector& OutPosition, FQuat& OutOrientation) const
{
}

void FRemoteControlHMD::GetCurrentPose(FQuat& CurrentOrientation)
{
	IRemoteControl& RemoteControl = IRemoteControl::Get();
	CurrentOrientation = BaseOrientation * RemoteControl.GetHeadTransform().Quaternion();
}

void FRemoteControlHMD::GetCurrentOrientationAndPosition(FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	CurrentPosition = FVector(0.0f, 0.0f, 0.0f);

	GetCurrentPose(CurrentOrientation);
	CurHmdOrientation = LastHmdOrientation = CurrentOrientation;
}

TSharedPtr<ISceneViewExtension, ESPMode::ThreadSafe> FRemoteControlHMD::GetViewExtension()
{
	TSharedPtr<FRemoteControlHMD, ESPMode::ThreadSafe> ptr(AsShared());
	return StaticCastSharedPtr<ISceneViewExtension>(ptr);
}

void FRemoteControlHMD::ApplyHmdRotation(APlayerController* PC, FRotator& ViewRotation)
{
	ViewRotation.Normalize();

	GetCurrentPose(CurHmdOrientation);
	LastHmdOrientation = CurHmdOrientation;

	const FRotator DeltaRot = ViewRotation - PC->GetControlRotation();
	DeltaControlRotation = (DeltaControlRotation + DeltaRot).GetNormalized();

	// Pitch from other sources is never good, because there is an absolute up and down that must be respected to avoid motion sickness.
	// Same with roll.
	DeltaControlRotation.Pitch = 0;
	DeltaControlRotation.Roll = 0;
	DeltaControlOrientation = DeltaControlRotation.Quaternion();

	ViewRotation = FRotator(DeltaControlOrientation * CurHmdOrientation);
}

bool FRemoteControlHMD::UpdatePlayerCamera(FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	return false;
}

bool FRemoteControlHMD::IsChromaAbCorrectionEnabled() const
{
	return false;
}

bool FRemoteControlHMD::IsHeadTrackingAllowed() const
{
	if (GEngine && GEngine->GameViewport)
	{
		// Always true in stereoscopic 3D
		if (GEngine->IsStereoscopic3D(GEngine->GameViewport->GetGameViewport()))
		{
			return true;
		}
		
		return GetDefault<URemoteControlSettings>()->bEnableHeadTracking;
	}

	return true;
}

void FRemoteControlHMD::ResetOrientationAndPosition(float yaw)
{
	ResetOrientation(yaw);
	ResetPosition();
}

void FRemoteControlHMD::ResetOrientation(float Yaw)
{
	IRemoteControl& RemoteControl = IRemoteControl::Get();
	RemoteControl.ResetHeadTracker();

	SetBaseOrientation(FRotator(0.0f, Yaw, 0.0f).Quaternion());
}

void FRemoteControlHMD::ResetPosition()
{

}

void FRemoteControlHMD::SetClippingPlanes(float NCP, float FCP)
{
}

void FRemoteControlHMD::SetBaseRotation(const FRotator& BaseRot)
{
	SetBaseOrientation(FRotator(0.0f, BaseRot.Yaw, 0.0f).Quaternion());
}

FRotator FRemoteControlHMD::GetBaseRotation() const
{
	return GetBaseOrientation().Rotator();
}

void FRemoteControlHMD::SetBaseOrientation(const FQuat& BaseOrient)
{
	BaseOrientation = BaseOrient;
}

FQuat FRemoteControlHMD::GetBaseOrientation() const
{
	return FQuat::Identity;
}

void FRemoteControlHMD::DrawDistortionMesh_RenderThread(struct FRenderingCompositePassContext& Context, const FIntPoint& TextureSize)
{
	float ClipSpaceQuadZ = 0.0f;
	FMatrix QuadTexTransform = FMatrix::Identity;
	FMatrix QuadPosTransform = FMatrix::Identity;
	const FSceneView& View = Context.View;
	const FIntRect SrcRect = View.ViewRect;

	FRHICommandListImmediate& RHICmdList = Context.RHICmdList;
	const FSceneViewFamily& ViewFamily = *(View.Family);
	FIntPoint ViewportSize = ViewFamily.RenderTarget->GetSizeXY();
	RHICmdList.SetViewport(0, 0, 0.0f, ViewportSize.X, ViewportSize.Y, 1.0f);

	static const uint32 NumVerts = 8;
	static const uint32 NumTris = 4;

	static const FDistortionVertex Verts[8] =
	{
		// left eye
		{ FVector2D(-0.9f, -0.9f), FVector2D(0.0f, 1.0f), FVector2D(0.0f, 1.0f), FVector2D(0.0f, 1.0f), 1.0f, 0.0f },
		{ FVector2D(-0.1f, -0.9f), FVector2D(0.5f, 1.0f), FVector2D(0.5f, 1.0f), FVector2D(0.5f, 1.0f), 1.0f, 0.0f },
		{ FVector2D(-0.1f, 0.9f), FVector2D(0.5f, 0.0f), FVector2D(0.5f, 0.0f), FVector2D(0.5f, 0.0f), 1.0f, 0.0f },
		{ FVector2D(-0.9f, 0.9f), FVector2D(0.0f, 0.0f), FVector2D(0.0f, 0.0f), FVector2D(0.0f, 0.0f), 1.0f, 0.0f },
		// right eye
		{ FVector2D(0.1f, -0.9f), FVector2D(0.5f, 1.0f), FVector2D(0.5f, 1.0f), FVector2D(0.5f, 1.0f), 1.0f, 0.0f },
		{ FVector2D(0.9f, -0.9f), FVector2D(1.0f, 1.0f), FVector2D(1.0f, 1.0f), FVector2D(1.0f, 1.0f), 1.0f, 0.0f },
		{ FVector2D(0.9f, 0.9f), FVector2D(1.0f, 0.0f), FVector2D(1.0f, 0.0f), FVector2D(1.0f, 0.0f), 1.0f, 0.0f },
		{ FVector2D(0.1f, 0.9f), FVector2D(0.5f, 0.0f), FVector2D(0.5f, 0.0f), FVector2D(0.5f, 0.0f), 1.0f, 0.0f },
	};

	static const uint16 Indices[12] = { /*Left*/ 0, 1, 2, 0, 2, 3, /*Right*/ 4, 5, 6, 4, 6, 7 };

	DrawIndexedPrimitiveUP(Context.RHICmdList, PT_TriangleList, 0, NumVerts, NumTris, &Indices,
		sizeof(Indices[0]), &Verts, sizeof(Verts[0]));
}

bool FRemoteControlHMD::IsStereoEnabled() const
{
	return true;
}

bool FRemoteControlHMD::EnableStereo(bool stereo)
{
	return true;
}

void FRemoteControlHMD::AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const
{
	SizeX = SizeX / 2;
	if( StereoPass == eSSP_RIGHT_EYE )
	{
		X += SizeX;
	}
}

void FRemoteControlHMD::CalculateStereoViewOffset(const enum EStereoscopicPass StereoPassType, const FRotator& ViewRotation, const float WorldToMeters, FVector& ViewLocation)
{
	if( StereoPassType != eSSP_FULL)
	{
		float EyeOffset = 3.20000005f;
		const float PassOffset = (StereoPassType == eSSP_LEFT_EYE) ? EyeOffset : -EyeOffset;
		ViewLocation += ViewRotation.Quaternion().RotateVector(FVector(0,PassOffset,0));
	}
}

FMatrix FRemoteControlHMD::GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType, const float FOV) const
{
	const float ProjectionCenterOffset = 0.151976421f;
	const float PassProjectionOffset = (StereoPassType == eSSP_LEFT_EYE) ? ProjectionCenterOffset : -ProjectionCenterOffset;

	const float HalfFov = 2.19686294f / 2.f;
	const float InWidth = 640.f;
	const float InHeight = 480.f;
	const float XS = 1.0f / tan(HalfFov);
	const float YS = InWidth / tan(HalfFov) / InHeight;

	const float InNearZ = GNearClippingPlane;
	return FMatrix(
		FPlane(XS,                      0.0f,								    0.0f,							0.0f),
		FPlane(0.0f,					YS,	                                    0.0f,							0.0f),
		FPlane(0.0f,	                0.0f,								    0.0f,							1.0f),
		FPlane(0.0f,					0.0f,								    InNearZ,						0.0f))

		* FTranslationMatrix(FVector(PassProjectionOffset,0,0));
}

void FRemoteControlHMD::InitCanvasFromView(FSceneView* InView, UCanvas* Canvas)
{
}

void FRemoteControlHMD::GetEyeRenderParams_RenderThread(const FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const
{
	EyeToSrcUVOffsetValue = FVector2D::ZeroVector;
	EyeToSrcUVScaleValue = FVector2D(1.0f, 1.0f);
}


void FRemoteControlHMD::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	InViewFamily.EngineShowFlags.MotionBlur = 0;
	InViewFamily.EngineShowFlags.HMDDistortion = true;
	InViewFamily.EngineShowFlags.SetScreenPercentage(true);
	InViewFamily.EngineShowFlags.StereoRendering = IsStereoEnabled();
}

void FRemoteControlHMD::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
	InView.BaseHmdOrientation = FQuat(FRotator(0.0f,0.0f,0.0f));
	InView.BaseHmdLocation = FVector(0.f);
//	WorldToMetersScale = InView.WorldToMetersScale;
	InViewFamily.bUseSeparateRenderTarget = false;
}

void FRemoteControlHMD::PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
	check(IsInRenderingThread());
}

void FRemoteControlHMD::PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily)
{
	check(IsInRenderingThread());
}

FRemoteControlHMD::FRemoteControlHMD() :
	CurHmdOrientation(FQuat::Identity),
	LastHmdOrientation(FQuat::Identity),
	DeltaControlRotation(FRotator::ZeroRotator),
	DeltaControlOrientation(FQuat::Identity),
	LastSensorTime(-1.0)
{
	BaseOrientation = FQuat::Identity;
	InterpupillaryDistance = 0.064f;
}

FRemoteControlHMD::~FRemoteControlHMD()
{
}

bool FRemoteControlHMD::IsInitialized() const
{
	return true;
}