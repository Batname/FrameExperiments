#pragma once

#include "ModuleManager.h"

class IRemoteControl
	: public IModuleInterface
{

public:
	static inline IRemoteControl& Get()
	{
		return FModuleManager::LoadModuleChecked< IRemoteControl >( "RemoteControl" );
	}

	virtual void StartBroadcast()=0;
	virtual void StopBroadcast()=0;
	virtual void StartBroadcastWithCustomIp(FString Ip)=0;
	virtual FRotator GetHeadTransform()=0;
	virtual void ResetHeadTracker()=0;
};

