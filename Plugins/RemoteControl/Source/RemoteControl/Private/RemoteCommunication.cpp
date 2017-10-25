#include "RemoteControlPrivatePCH.h"
#include "RemoteCommunication.h"

FRemoteCommunication::FRemoteCommunication(FRemoteControl* InContext, bool bInTCP, const FString& InIp, int32 InPort, const FString& DebugDescription)
	:Context(InContext)
{
	bIsTCP = bInTCP;
	Ip = InIp;
	Port = InPort;
	Socket = nullptr;
	bIsConnected = false;
	TCPConnector = nullptr;
	TCPReceiver = nullptr;
	TCPSender = nullptr;
	ControllerId = 0;
	LastPingTime = 0.f;
	LastSetControllerTime = 0.f;

	auto SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	FIPv4Address::Parse(Ip, RemoteAdress);
	RemoteEndPoint = FIPv4Endpoint(RemoteAdress, Port);

	if (bIsTCP)
	{
		Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("Remote Control"));

		TCPConnector = new FRemoteTCPConnector(this, Socket, RemoteEndPoint.ToInternetAddr());
		TCPConnector->OnConnect().BindRaw(this, &FRemoteCommunication::OnTCPConnect);
		TCPConnector->Start();
	}
	else
	{
		// UDP always connected!
		bIsConnected = true;
	}
	
	// Required. We need to wait clients because clients can send their controller id
	ForceClientControllerId();
}

FRemoteCommunication::~FRemoteCommunication()
{
	if (Socket != nullptr)
	{
		Socket->Close();
		auto SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}
}

void FRemoteCommunication::Stop()
{
	if (bIsConnected)
	{
		// Send close signal!
		SendAction(REMOTE_ACTION_CLOSE);

		// @todo: Close the socket before the call ->Stop functions
		if (Socket)
		{
			Socket->Close();
		}
		
		if (TCPConnector)
		{
			TCPConnector->Stop();
		}
		
		if (TCPSender)
		{
			TCPSender->Stop();
		}
		if (TCPReceiver)
		{
			TCPReceiver->Stop();
		}
	}
}

void FRemoteCommunication::SendAction(uint32 InAction)
{
	if (bIsConnected)
	{
		int32 DatLen = 0;

		FBufferArchive Buf;
		Buf << InAction;
		Buf << DatLen;

		TArray<uint8>* FinalData = new TArray<uint8>();
		FinalData->AddUninitialized(Buf.Num());
		FMemory::Memcpy(FinalData->GetData(), Buf.GetData(), Buf.Num());

		if (bIsTCP && TCPSender)
		{
			TCPSender->Send(MakeShareable(FinalData));
		}
		else if (Context->GetUDPSender())
		{
			Context->GetUDPSender()->Send(MakeShareable(FinalData), RemoteEndPoint);
		}
	}
}

void FRemoteCommunication::SendAction(uint32 InAction, const FBufferArchive& InData)
{
	if (bIsConnected)
	{
		int32 DatLen = InData.Num();

		FBufferArchive Buf;
		Buf << InAction;
		Buf << DatLen;
		Buf.Append(InData.GetData(), InData.Num());

		TArray<uint8>* FinalData = new TArray<uint8>();
		FinalData->AddUninitialized(Buf.Num());
		FMemory::Memcpy(FinalData->GetData(), Buf.GetData(), Buf.Num());

		if (bIsTCP && TCPSender)
		{
			TCPSender->Send(MakeShareable(FinalData));
		}
		else if (Context->GetUDPSender())
		{
			Context->GetUDPSender()->Send(MakeShareable(FinalData), RemoteEndPoint);
		}
	}
}

void FRemoteCommunication::ForceClientControllerId()
{
	LastSetControllerTime = FApp::GetCurrentTime();
}

void FRemoteCommunication::SetControllerId(int32 InControllerId)
{
	ControllerId = InControllerId;
	ForceClientControllerId();
	Sync();
}

void FRemoteCommunication::Sync()
{
	FBufferArchive Data;
	Data << ControllerId;

	SendAction(REMOTE_ACTION_SYNC, Data);
}

void FRemoteCommunication::Ping()
{
	const double CurrentTime = FApp::GetCurrentTime();
	if ((CurrentTime - LastPingTime) >= REMOTE_PING_RATE)
	{
		LastPingTime = FApp::GetCurrentTime();
		SendAction(REMOTE_ACTION_PING);
	}
}

void FRemoteCommunication::Pong()
{
	SendAction(REMOTE_ACTION_PONG);
}

void FRemoteCommunication::OnTCPConnect(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		bIsConnected = true;

		// Required. We need to wait clients because clients can send their controller id
		ForceClientControllerId();

		TCPSender = new FRemoteTCPSender(this, Socket, TEXT("Remote Control - TCP Sender"));
		TCPReceiver = new FRemoteTCPReceiver(this, Socket, TEXT("Remote Control - TCP Receiver"));
		TCPReceiver->OnReceived().BindLambda([&](const FArrayReaderPtr& Data) {
			OnReceived(true, Data);
		});
	}
}

void FRemoteCommunication::OnReceived(bool bInIsTCP, const FArrayReaderPtr& Data)
{
	if (!bInIsTCP)
	{
		Buffer.Empty();
	}

	if (Data->Num() > 0)
	{
		Buffer.Append(Data->GetData(), Data->Num());
	}

	if (Buffer.Num() >= 9)
	{
		bool bNext = false;
		do {
			bNext = false;

			FBufferReader Input(Buffer.GetData(), Buffer.Num(), false);

			uint8 NewControllerId = 0;
			int32 Action = 0;
			int32 DatLen = 0;

			Input << NewControllerId;
			Input << Action;
			Input << DatLen;

			// Wait little time to allow to sync devices.
			if ((FApp::GetCurrentTime() - LastSetControllerTime) > REMOTE_WAIT_CLIENT_CONTROLLER)
			{		
				if (ControllerId != NewControllerId)
				{
					int32 OldControllerId = ControllerId;
					ControllerId = NewControllerId;
					Context->OnControllerChanged(this, OldControllerId, NewControllerId);
				}
			}

			if ((Input.TotalSize() - Input.Tell()) >= DatLen)
			{
				if (DatLen > 0)
				{
					// Prev seek. So reader can read Array!
					Input.Seek(Input.Tell() - 4);

					TArray<uint8> DataBuf;
					Input << DataBuf;

					OnAction(Action, DataBuf);
				}
				else
				{
					OnAction(Action, TArray<uint8>());
				}

				if ((Input.TotalSize() - Input.Tell()) >= 9)
				{
					bNext = true;
				}

				// Slice buffer
				Buffer.RemoveAt(0, Input.Tell());

			}
		} while (bNext);
	}
}

void FRemoteCommunication::OnAction(uint32 Action, const TArray<uint8>& Data)
{
	if (Action == REMOTE_ACTION_PING)
	{
		Pong();
	}
	else
	{
		Context->OnActionReceived(ControllerId, Action, Data);
	}
}
