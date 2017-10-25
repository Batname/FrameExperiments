// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Runtime/SlateCore/Public/Layout/Geometry.h"
#include "Runtime/RHI/Public/RHI.h"
#include "FrameExperimentsPawn.generated.h"

UCLASS(Config=Game)
class AFrameExperimentsPawn : public APawn
{
	GENERATED_BODY()

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PlaneMesh;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;
public:
	AFrameExperimentsPawn();

	// Begin AActor overrides
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End AActor overrides

	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	void OnSlateRendered(class SWindow& SlateWindow, void* ViewportRHIPtr);

protected:

	// Begin APawn overrides
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
	// End APawn overrides

	/** Bound to the thrust axis */
	void ThrustInput(float Val);
	
	/** Bound to the vertical axis */
	void MoveUpInput(float Val);

	/** Bound to the horizontal axis */
	void MoveRightInput(float Val);

private:

	/** How quickly forward speed changes */
	UPROPERTY(Category=Plane, EditAnywhere)
	float Acceleration;

	/** How quickly pawn can steer */
	UPROPERTY(Category=Plane, EditAnywhere)
	float TurnSpeed;

	/** Max forward speed */
	UPROPERTY(Category = Pitch, EditAnywhere)
	float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category=Yaw, EditAnywhere)
	float MinSpeed;

	/** Current forward speed */
	float CurrentForwardSpeed;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current roll speed */
	float CurrentRollSpeed;

public:
	/** Returns PlaneMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetPlaneMesh() const { return PlaneMesh; }
	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }

// battest
private:
	class UTexture2D* MyTex = nullptr;

	void* ReadbackBuffer;

	bool bIsBufferReady = false;

	UMaterialInstanceDynamic* DynamicMatInstance;


	APlayerController* PlayerController;

	FVector2D BatPosition = FVector2D::ZeroVector;
	FVector2D BatSize = FVector2D::ZeroVector;
	bool FindViewportGeometry_Bat(TSharedPtr<SWindow> WindowWidget, FGeometry& OutGeometry)const;
	bool FindViewportGeometryInternal_Bat(const FGeometry& Geometry, TSharedPtr<SWidget> Widget, FGeometry& OutGeometry)const;


	void UpdateTextureRegions(class UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);


	// Dynamic texture methods
	void CreateTexture(bool argForceMake);
	void UpdateTexture();

	// update class instances in the editor if changes are made to their properties
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called after the actors components have been initialized
	virtual void PostInitializeComponents() override;


	// dTex res
	int dtWidth;
	int dtHeight;
	int dtBytesPerPixel;

	// dTex buffer
	uint8 *dtBuffer;
	int dtBufferSize;
	int dtBufferSizeSqrt;

	FUpdateTextureRegion2D* dtUpdateTextureRegion;
};