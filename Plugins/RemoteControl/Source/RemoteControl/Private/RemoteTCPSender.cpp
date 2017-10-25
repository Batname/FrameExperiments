#include "RemoteControlPrivatePCH.h"
#include "RemoteTCPSender.h"

FRemoteTCPSender::FRemoteTCPSender(FRemoteCommunication* InContext, FSocket* InSocket, const TCHAR* ThreadDescription)
	: Context(InContext)
{
	SendRate = 0;
	Socket = InSocket;
	Stopping = false;
	WaitTime = FTimespan::FromMilliseconds(100);

	check(Socket != NULL);
	check(Socket->GetSocketType() == SOCKTYPE_Streaming);

	int32 NewSize = 0;
	Socket->SetSendBufferSize(512 * 1024, NewSize);

	WorkEvent = FPlatformProcess::GetSynchEventFromPool();

	Thread = FRunnableThread::Create(this, ThreadDescription, 128 * 1024, TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask());
}

FRemoteTCPSender::~FRemoteTCPSender()
{
	if (Thread != NULL)
	{
		Thread->Kill(true);
		delete Thread;
	}

	FPlatformProcess::ReturnSynchEventToPool(WorkEvent);
	WorkEvent = nullptr;
}

bool FRemoteTCPSender::Send(const TSharedRef<TArray<uint8>, ESPMode::ThreadSafe>& Data)
{
	if (!Stopping && SendQueue.Enqueue(FPacket(Data)))
	{
		WorkEvent->Trigger();

		return true;
	}

	return false;
}

uint32 FRemoteTCPSender::Run()
{
	while (!Stopping)
	{
		while (!Stopping && !SendQueue.IsEmpty())
		{
			if (Socket->Wait(ESocketWaitConditions::WaitForWrite, WaitTime))
			{
				FPacket Packet;
				int32 Sent = 0;

				SendQueue.Dequeue(Packet);
				Socket->Send(Packet.Data->GetData(), Packet.Data->Num(), Sent);

				if (Sent != Packet.Data->Num())
				{
					Stopping = true;

					return 0;
				}
			}
		}

		WorkEvent->Wait(WaitTime);
	}
	
	Socket->Close();

	return 0;
}

void FRemoteTCPSender::Stop()
{
	Stopping = true;

	WorkEvent->Trigger();
	Thread->WaitForCompletion();
}
