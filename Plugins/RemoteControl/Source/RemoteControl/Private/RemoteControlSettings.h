#pragma once

#include "RemoteControlSettings.generated.h"

UENUM(BlueprintType)
enum class ERemoteResolution : uint8
{
	r128 UMETA(DisplayName = "128"),
	r256 UMETA(DisplayName = "256"),
	r512 UMETA(DisplayName = "512"),
	r768 UMETA(DisplayName = "768"),
	r1024 UMETA(DisplayName = "1024")
};

UENUM(BlueprintType)
enum class ERemoteControlCommunication : uint8
{
	USB,
	WiFi UMETA(DisplayName = "Wi-Fi")
};

UCLASS(config = EditorSettings)
class REMOTECONTROL_API URemoteControlSettings : public UObject
{
	GENERATED_BODY()

public:

	/** Enable remote control. */
	UPROPERTY(Config, EditAnywhere, Category = GeneralSettings)
	bool bIsEnabled;

	/** Frame rate of stream. Do not change this to high unless you are sure your mobile device can handle it. Recommended: 25, 30 */
	UPROPERTY(Config, EditAnywhere, Category = GeneralSettings, Meta = (UIMin = 5, ClampMin = 1, UIMax = 60))
	int32 FrameRate;
	
	/**
	* If you change this setting, you need to re-start stream.
	* If you prefer high resolution make sure your mobile device can handle it.
	*/
	UPROPERTY(Config, EditAnywhere, Category = GeneralSettings)
	ERemoteResolution Resolution;
	
	/**
	Quality of compressed stream.
	Low: Fastest but lower quality
	High: Slowest but highest quality.
	
	Do not change this setting too high unless you are sure your mobile device can handle it. Recommended: 60
	*/
	UPROPERTY(Config, EditAnywhere, Category = GeneralSettings, Meta = (UIMin = 5, ClampMin = 1, UIMax = 100))
	int32 Quality;

	/** @todo: Port is fixed and cannot be changed currently. */
	UPROPERTY(Config, BlueprintReadOnly, Category = GeneralSettings)
	int32 Port;

	/**
	 !!! IMPORTANT !!! iOS devices doesn't support USB communication. Will be support in the future releases.

	 Communication mode between your mobile device and editor.
	*/
	UPROPERTY(Config, EditAnywhere, Category = FirstController)
	ERemoteControlCommunication Communication;

	/** If you want to connect your mobile device via Wi-Fi, you need put ip address of the device. */
	UPROPERTY(Config, EditAnywhere, Category = FirstController)
	FString IpAddress;
	
	/**
	If you want to enable additional controllers, check it.
	*/
	UPROPERTY(Config, EditAnywhere, Category = AdditionalControllers)
	bool bEnableAdditionalControllers;
	
	/** Please enter ip address of each device for additional controller. Additional controllers doesn't support USB communication. */
	UPROPERTY(Config, EditAnywhere, Category = AdditionalControllers)
	TArray<FString> AdditionalControllers;
	
	/** Only affects editor viewport (Not affects Launch VR Preview. Always true in VR Preview) */
	UPROPERTY(Config, EditAnywhere, Category = VR)
	bool bEnableHeadTracking;

	URemoteControlSettings(const FObjectInitializer& ObjectInitializer);

	int32 GetRealQuality() const
	{
		return FMath::GetMappedRangeValueClamped(FVector2D(5, 100), FVector2D(5, 50), Quality);
	}
};

