#pragma once

#include "RemoteControlSettings.h"
#include "Networking.h"

class FRemoteCommunication;

//////////////////////////////////////////////////////////////////////////////////////////////////

class FRemoteTCPConnector
	: public FRunnable
{
public:
	DECLARE_DELEGATE_OneParam(FOnConnectSignature, bool);

protected:
	TSharedPtr<FRemoteCommunication> Context;
	FSocket* Socket;
	TSharedRef<FInternetAddr> Addr;
	FTimespan WaitTime;
	FOnConnectSignature OnConnectDelegate;
	FDelegateHandle OnConnectHandle;
	
public:
	FRemoteTCPConnector(FRemoteCommunication* InContext, FSocket* InSocket, TSharedRef<FInternetAddr> InAddr);
	~FRemoteTCPConnector();

	FOnConnectSignature& OnConnect() { return OnConnectDelegate; }

	void Start();

	// Begin FRunnable interface
	virtual bool Init() override;
	virtual uint32 Run();
	virtual void Stop() override;
	// End FRunnable interface

private:
	// Holds the thread object.
	FRunnableThread* Thread;
};
