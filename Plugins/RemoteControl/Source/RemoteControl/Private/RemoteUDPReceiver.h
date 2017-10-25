#pragma once

#include "RemoteControlSettings.h"
#include "Networking.h"

class FRemoteCommunication;

//////////////////////////////////////////////////////////////////////////////////////////////////

class FRemoteUDPReceiver
	: public FRunnable
{
	DECLARE_DELEGATE_TwoParams(FOnReceivedSignature, const FArrayReaderPtr&, const FIPv4Endpoint&);

protected:
	FSocket* Socket;
	FTimespan WaitTime;
	bool bIsStopping;
	FOnReceivedSignature OnReceivedDelegate;

public:
	FRemoteUDPReceiver(FSocket* InSocket, const TCHAR* ThreadDescription);
	~FRemoteUDPReceiver();

	FOnReceivedSignature& OnReceived() { return OnReceivedDelegate; }

	// Begin FRunnable interface
	virtual bool Init() override;
	virtual uint32 Run();
	virtual void Stop() override;
	// End FRunnable interface

private:
	// Holds the thread object.
	FRunnableThread* Thread;
};
