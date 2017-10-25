#include "RemoteControlPrivatePCH.h"
#include "RemoteUDPReceiver.h"

FRemoteUDPReceiver::FRemoteUDPReceiver(FSocket* InSocket, const TCHAR* ThreadDescription)
	: Socket(InSocket)
	, WaitTime(FTimespan::FromMilliseconds(100))
	, bIsStopping(false)
{

	Thread = FRunnableThread::Create(this, ThreadDescription, 128 * 1024, TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask());
}

FRemoteUDPReceiver::~FRemoteUDPReceiver()
{
	if (Thread != NULL)
	{
		Thread->Kill(true);
		delete Thread;
	}
}

bool FRemoteUDPReceiver::Init()
{
	return true;
}

uint32 FRemoteUDPReceiver::Run()
{
	auto SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedPtr<FInternetAddr> RecvAddr = SocketSubsystem->CreateInternetAddr();
	
	while (!bIsStopping) {

		if (Socket->Wait(ESocketWaitConditions::WaitForRead, WaitTime))
		{
			uint32 Size;

			while (Socket->HasPendingData(Size))
			{
				FArrayReaderPtr Reader = MakeShareable(new FArrayReader(true));
				Reader->SetNumUninitialized(FMath::Min(Size, 65507u));

				int32 Read = 0;
				Socket->RecvFrom(Reader->GetData(), Reader->Num(), Read, *RecvAddr);
				Reader->SetNum(Read);

				OnReceivedDelegate.ExecuteIfBound(Reader, FIPv4Endpoint(RecvAddr));
			}
		}
	}

	return 0;
}

void FRemoteUDPReceiver::Stop()
{
	bIsStopping = true;
	Thread->WaitForCompletion();
}

