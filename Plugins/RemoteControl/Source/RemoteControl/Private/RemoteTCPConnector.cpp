#include "RemoteControlPrivatePCH.h"
#include "RemoteTCPConnector.h"

FRemoteTCPConnector::FRemoteTCPConnector(FRemoteCommunication* InContext, FSocket* InSocket, TSharedRef<FInternetAddr> InAddr)
	: Context(InContext)
	, Socket(InSocket)
	, Addr(InAddr)
	, WaitTime(5000.f)
{
	check(InSocket != nullptr);

}

FRemoteTCPConnector::~FRemoteTCPConnector()
{
	if (Thread != NULL)
	{
		Thread->Kill(true);
		delete Thread;
	}
}

bool FRemoteTCPConnector::Init()
{
	return true;
}

void FRemoteTCPConnector::Start()
{
	Thread = FRunnableThread::Create(this, TEXT("Remote Control"), 128 * 1024, TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask());
}

uint32 FRemoteTCPConnector::Run()
{
	Socket->SetNonBlocking(false);

	const bool bResult = Socket->Connect(*Addr);
	OnConnectDelegate.ExecuteIfBound(bResult);

	return 0;
}

void FRemoteTCPConnector::Stop()
{
	Socket->Close();
	Thread->WaitForCompletion();
}
