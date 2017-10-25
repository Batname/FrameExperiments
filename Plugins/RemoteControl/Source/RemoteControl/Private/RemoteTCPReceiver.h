#pragma once

#include "RemoteControlSettings.h"
#include "Networking.h"

class FRemoteCommunication;

class FRemoteTCPReceiver
	: public FRunnable
{
	DECLARE_DELEGATE_OneParam(FOnReceivedSignature, const FArrayReaderPtr&);

protected:
	TSharedPtr<FRemoteCommunication> Context;
	FSocket* Socket;
	FTimespan WaitTime;
	bool bIsStopping;
	FOnReceivedSignature OnReceivedDelegate;

public:
	FRemoteTCPReceiver(FRemoteCommunication* InContext, FSocket* InSocket, const TCHAR* ThreadDescription);
	~FRemoteTCPReceiver();

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
