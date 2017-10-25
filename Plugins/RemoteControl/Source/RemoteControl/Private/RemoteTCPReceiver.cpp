#include "RemoteControlPrivatePCH.h"
#include "RemoteTCPReceiver.h"

FRemoteTCPReceiver::FRemoteTCPReceiver(FRemoteCommunication* InContext, FSocket* InSocket, const TCHAR* ThreadDescription)
	: Context(InContext)
	, Socket(InSocket)
	, WaitTime(FTimespan::FromMilliseconds(100))
	, bIsStopping(false)
{

	Thread = FRunnableThread::Create(this, ThreadDescription, 128 * 1024, TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask());
}

FRemoteTCPReceiver::~FRemoteTCPReceiver()
{
	if (Thread != NULL)
	{
		Thread->Kill(true);
		delete Thread;
	}
}

bool FRemoteTCPReceiver::Init()
{
	return true;
}

uint32 FRemoteTCPReceiver::Run()
{
	while (!bIsStopping) {

		if (!Socket->Wait(ESocketWaitConditions::WaitForRead, WaitTime))
		{
			// Receiver time out
			continue;
		}

		uint32 Size;

		while (Socket->HasPendingData(Size))
		{
			FArrayReaderPtr Reader = MakeShareable(new FArrayReader(true));
			Reader->SetNumUninitialized(FMath::Min(Size, 65507u));

			int32 Read = 0;
			Socket->Recv(Reader->GetData(), Reader->Num(), Read);
			Reader->SetNum(Read);

			OnReceivedDelegate.ExecuteIfBound(Reader);
		}
	}

	return 0;
}

void FRemoteTCPReceiver::Stop()
{
	bIsStopping = true;
	Thread->WaitForCompletion();
}
