#pragma once

#include "RemoteControlSettings.h"
#include "Networking.h"
#include "RemoteTCPConnector.h"
#include "RemoteTCPReceiver.h"
#include "RemoteTCPSender.h"
#include "RemoteUDPReceiver.h"
#include "RemoteUDPSender.h"

class FRemoteControl;

class FRemoteCommunication
{
protected:
	TSharedPtr<FRemoteControl> Context;
	bool bIsTCP;
	FString Ip;
	int32 Port;
	bool bIsConnected;

	FBufferArchive Buffer;

public:
	FRemoteCommunication(FRemoteControl* InContext, bool bInTCP, const FString& InIp, int32 InPort, const FString& DebugDescription = TEXT(""));
	virtual ~FRemoteCommunication();

	virtual void Stop();

	void OnTCPConnect(bool bWasSuccessful);
	void OnReceived(bool bInIsTCP, const FArrayReaderPtr& Data);
	
	void OnAction(uint32 Action, const TArray<uint8>& Data);
	void SendAction(uint32 InAction);
	void SendAction(uint32 InAction, const FBufferArchive& InData);
	
	bool IsConnected()const { return bIsConnected; }

	void SetControllerId(int32 InControllerId);
	uint8 GetControllerId()const { return ControllerId; }
	void ForceClientControllerId();

	bool IsUdp()const { return !bIsTCP; }
	
	FIPv4Endpoint GetRemoteEndPoint()const { return RemoteEndPoint; }

	void Sync();
	void Ping();
	void Pong();
private:
	FSocket* Socket;
	FIPv4Endpoint RemoteEndPoint;
	FIPv4Address RemoteAdress;

	FRemoteTCPConnector* TCPConnector;
	FRemoteTCPReceiver* TCPReceiver;
	FRemoteTCPSender* TCPSender;
	
	uint8 ControllerId;

	double LastPingTime;
	double LastSetControllerTime;
};
