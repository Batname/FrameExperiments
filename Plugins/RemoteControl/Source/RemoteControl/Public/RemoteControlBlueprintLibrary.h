#pragma once

#include "RemoteControlModule.h"
#include "RemoteControlBlueprintLibrary.generated.h"

UCLASS()
class REMOTECONTROL_API URemoteControlBlueprintLibrary
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	URemoteControlBlueprintLibrary(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintCallable, Category = "Plugin - Remote Control")
	static void StartRemoteControl(FString Ip);

	UFUNCTION(BlueprintCallable, Category = "Plugin - Remote Control")
	static void StopRemoteControl();
};
