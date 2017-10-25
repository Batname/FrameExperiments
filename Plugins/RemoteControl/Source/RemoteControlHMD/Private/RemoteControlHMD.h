#pragma once
#include "IHeadMountedDisplay.h"
#include "Stats.h"
#include "SceneViewExtension.h"

class FRemoteControlHMDModule : public IRemoteControlVR
{
public:
	FRemoteControlHMDModule();

protected:
	bool Initialize();

public:
	static inline FRemoteControlHMDModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FRemoteControlHMDModule >( "RemoteControlHMD" );
	}

public:
	/** IModuleInterface */
	virtual void ShutdownModule() override;

	/** IHeadMountedDisplayModule */
	virtual FString GetModuleKeyName() const override;
	virtual bool PreInit() override;
	virtual bool IsHMDConnected() override;
	virtual int GetGraphicsAdapter() override;
	virtual FString GetAudioInputDevice() override;
	virtual FString GetAudioOutputDevice() override;
	virtual TSharedPtr< class IHeadMountedDisplay, ESPMode::ThreadSafe > CreateHeadMountedDisplay() override;

protected:
	bool bInitialized;
	bool bInitializeCalled;
	TWeakPtr< IHeadMountedDisplay, ESPMode::ThreadSafe > HeadMountedDisplay;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Simple Head Mounted Display
 */
class FRemoteControlHMD : public IHeadMountedDisplay, public ISceneViewExtension, public TSharedFromThis<FRemoteControlHMD, ESPMode::ThreadSafe>
{
protected:
	float InterpupillaryDistance;

public:
	/** IHeadMountedDisplay interface */
	virtual FName GetDeviceName() const override
	{
		static FName DefaultName(TEXT("RemoteControlVR"));
		return DefaultName;
	}

	virtual bool IsHMDConnected() override { return true; }
	virtual bool IsHMDEnabled() const override;
	virtual void EnableHMD(bool allow = true) override;
	virtual EHMDDeviceType::Type GetHMDDeviceType() const override;
	virtual bool GetHMDMonitorInfo(MonitorInfo&) override;
	
	virtual void OnScreenModeChange(EWindowMode::Type WindowMode) {}
	
	virtual void GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const override;

	virtual bool DoesSupportPositionalTracking() const override {return false;}
	virtual bool HasValidTrackingPosition() override {return false;}
	virtual void GetPositionalTrackingCameraProperties(FVector& OutOrigin, FQuat& OutOrientation, float& OutHFOV, float& OutVFOV, float& OutCameraDistance, float& OutNearPlane, float& OutFarPlane) const override;
	virtual void RebaseObjectOrientationAndPosition(FVector& OutPosition, FQuat& OutOrientation) const override;

	virtual void SetInterpupillaryDistance(float NewInterpupillaryDistance) override {InterpupillaryDistance = NewInterpupillaryDistance;}
	virtual float GetInterpupillaryDistance() const override {return InterpupillaryDistance;}

	virtual void GetCurrentOrientationAndPosition(FQuat& CurrentOrientation, FVector& CurrentPosition) override;
	virtual TSharedPtr<class ISceneViewExtension, ESPMode::ThreadSafe> GetViewExtension() override;
	virtual void ApplyHmdRotation(APlayerController* PC, FRotator& ViewRotation) override;
	virtual bool UpdatePlayerCamera(FQuat& CurrentOrientation, FVector& CurrentPosition) override;

	virtual bool IsChromaAbCorrectionEnabled() const override;

	virtual bool IsPositionalTrackingEnabled() const override {return true;}
	//virtual bool EnablePositionalTracking(bool enable) override;

	virtual bool IsHeadTrackingAllowed() const override;

	//virtual bool IsInLowPersistenceMode() const override;
	//virtual void EnableLowPersistenceMode(bool Enable = true) override;

	virtual void ResetOrientationAndPosition(float yaw = 0.f) override;
	virtual void ResetOrientation(float Yaw = 0.f) override;
	virtual void ResetPosition() override;

	virtual void SetClippingPlanes(float NCP, float FCP) override;

	virtual void SetBaseRotation(const FRotator& BaseRot) override;
	virtual FRotator GetBaseRotation() const override;

	virtual void SetBaseOrientation(const FQuat& BaseOrient) override;
	virtual FQuat GetBaseOrientation() const override;

	virtual void DrawDistortionMesh_RenderThread(struct FRenderingCompositePassContext& Context, const FIntPoint& TextureSize) override;

	/** IStereoRendering interface */
	virtual bool IsStereoEnabled() const override;
	virtual bool EnableStereo(bool stereo = true) override;
	virtual void AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const override;
	virtual void CalculateStereoViewOffset(const EStereoscopicPass StereoPassType, const FRotator& ViewRotation,
		const float MetersToWorld, FVector& ViewLocation) override;
	virtual FMatrix GetStereoProjectionMatrix(const EStereoscopicPass StereoPassType, const float FOV) const override;
	virtual void InitCanvasFromView(FSceneView* InView, UCanvas* Canvas) override;
	virtual void GetEyeRenderParams_RenderThread(const struct FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const override;

	/** ISceneViewExtension interface */
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) {}
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override;
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override;

public:
	/** Constructor */
	FRemoteControlHMD();

	/** Destructor */
	virtual ~FRemoteControlHMD();

	/** @return	True if the HMD was initialized OK */
	bool IsInitialized() const;

private:

	FQuat					CurHmdOrientation;
	FQuat					LastHmdOrientation;

	FRotator				DeltaControlRotation;    // same as DeltaControlOrientation but as rotator
	FQuat					DeltaControlOrientation; // same as DeltaControlRotation but as quat

	double					LastSensorTime;

	FQuat		BaseOrientation;

	void GetCurrentPose(FQuat& CurrentOrientation);
};


DEFINE_LOG_CATEGORY_STATIC(LogRemoteControlHMD, Log, All);
