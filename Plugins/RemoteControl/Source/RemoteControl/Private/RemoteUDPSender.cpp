#include "RemoteControlPrivatePCH.h"
#include "RemoteUDPSender.h"

FRemoteUDPSender::FRemoteUDPSender(FSocket* InSocket, const TCHAR* ThreadDescription)
{
	SendRate = 0;
	Socket = InSocket;
	Stopping = false;
	WaitTime = FTimespan::FromMilliseconds(100);

	check(Socket != NULL);
	check(Socket->GetSocketType() == SOCKTYPE_Datagram);

	int32 NewSize = 0;
	Socket->SetSendBufferSize(512 * 1024, NewSize);

	WorkEvent = FPlatformProcess::GetSynchEventFromPool();

	Thread = FRunnableThread::Create(this, ThreadDescription, 128 * 1024, TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask());
}

FRemoteUDPSender::~FRemoteUDPSender()
{
	if (Thread != NULL)
	{
		Thread->Kill(true);
		delete Thread;
	}

	FPlatformProcess::ReturnSynchEventToPool(WorkEvent);
	WorkEvent = nullptr;
}

bool FRemoteUDPSender::Send(const TSharedRef<TArray<uint8>, ESPMode::ThreadSafe>& Data, const FIPv4Endpoint& Recipient)
{
	if (!Stopping && SendQueue.Enqueue(FPacket(Data, Recipient)))
	{
		WorkEvent->Trigger();

		return true;
	}

	return false;
}

uint32 FRemoteUDPSender::Run()
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
				Socket->SendTo(Packet.Data->GetData(), Packet.Data->Num(), Sent, *Packet.Recipient.ToInternetAddr());
			}
		}

		WorkEvent->Wait(WaitTime);
	}
	
	// Send remaining data
	while (!SendQueue.IsEmpty())
	{
		if (Socket->Wait(ESocketWaitConditions::WaitForWrite, WaitTime))
		{
			FPacket Packet;
			int32 Sent = 0;

			SendQueue.Dequeue(Packet);
			Socket->SendTo(Packet.Data->GetData(), Packet.Data->Num(), Sent, *Packet.Recipient.ToInternetAddr());
		}
		else
		{
			// Failed to send remaining packets...
			break;
		}
	}

	return 0;
}

void FRemoteUDPSender::Stop()
{
	Stopping = true;
	WorkEvent->Trigger();
	Thread->WaitForCompletion();
}
