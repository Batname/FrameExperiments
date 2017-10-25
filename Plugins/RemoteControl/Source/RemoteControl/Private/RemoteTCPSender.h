#pragma once

#include "Networking.h"


class FRemoteTCPSender
	: public FRunnable
{
	// Structure for outbound packets.
	struct FPacket
	{
		// Holds the packet's data.
		TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> Data;

		// Default constructor.
		FPacket() { }

		// Creates and initializes a new instance.
		FPacket(const TSharedRef<TArray<uint8>, ESPMode::ThreadSafe>& InData)
			: Data(InData)
		{ }
	};

protected:
	TSharedPtr<FRemoteCommunication> Context;
	FSocket* Socket;

public:

	/**
	* Creates and initializes a new socket sender.
	*
	* @param InSocket - The UDP socket to use for sending data.
	* @param ThreadDescription - The thread description text (for debugging).
	*/
	FRemoteTCPSender(FRemoteCommunication* InContext, FSocket* InSocket, const TCHAR* ThreadDescription);

	/**
	* Destructor.
	*/
	~FRemoteTCPSender();

public:

	/**
	* Gets the maximum send rate (in bytes per second).
	*
	* @return Current send rate.
	*/
	uint32 GetSendRate() const
	{
		return SendRate;
	}

	/**
	* Gets the current throughput (in bytes per second).
	*
	* @return Current throughput.
	*/
	uint32 GetThroughput() const
	{
		return Throughput;
	}

	/**
	* Sends data to the specified recipient.
	*
	* @param Data - The data to send.
	* @param Recipient - The recipient.
	*
	* @return true if the data will be sent, false otherwise.
	*/
	bool Send(const TSharedRef<TArray<uint8>, ESPMode::ThreadSafe>& Data);

	/**
	* Sets the send rate (in bytes per second).
	*
	* @param Rate - The new send rate (0 = unlimited).
	*/
	void SetSendRate(uint32 Rate)
	{
		SendRate = Rate;
	}

	/**
	* Sets the maximum time span to wait for work items.
	*
	* @param Timespan - The wait time.
	*/
	void SetWaitTime(const FTimespan& Timespan)
	{
		WaitTime = Timespan;
	}

public:

	virtual bool Init() override
	{
		return true;
	}

	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override { }

private:

	// Holds the send queue.
	TQueue<FPacket, EQueueMode::Mpsc> SendQueue;

	// Holds the send rate.
	uint32 SendRate;

	// Holds a flag indicating that the thread is stopping.
	bool Stopping;

	// Holds the thread object.
	FRunnableThread* Thread;

	// Holds the current throughput.
	uint32 Throughput;

	// Holds the amount of time to wait for inbound packets.
	FTimespan WaitTime;

	// Holds an event signaling that inbound messages need to be processed.
	FEvent* WorkEvent;
};