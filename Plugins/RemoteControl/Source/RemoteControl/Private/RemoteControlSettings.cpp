#include "RemoteControlPrivatePCH.h"
#include "RemoteControlSettings.h"


URemoteControlSettings::URemoteControlSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsEnabled = true;
	FrameRate = 25;
	Resolution = ERemoteResolution::r512;
	Quality = 60;
	Communication = ERemoteControlCommunication::USB;
	IpAddress = TEXT("");
	Port = 56789;
	bEnableHeadTracking = false;
}
