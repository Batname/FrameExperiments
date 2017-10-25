#include "RemoteControlPrivatePCH.h"
#include "IRemoteControl.h"
#include "RemoteControlBlueprintLibrary.h"

URemoteControlBlueprintLibrary::URemoteControlBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void URemoteControlBlueprintLibrary::StartRemoteControl(FString Ip)
{
	IRemoteControl& RemoteControl = IRemoteControl::Get();
	RemoteControl.StartBroadcastWithCustomIp(Ip);
}

void URemoteControlBlueprintLibrary::StopRemoteControl()
{
	IRemoteControl& RemoteControl = IRemoteControl::Get();
	RemoteControl.StopBroadcast();
}
