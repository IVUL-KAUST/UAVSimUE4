/*

RamaUDPReceiver

by Rama
*/
//#include "Runtime/Networking/Public/
//#include "Runtime/Networking/Public/Common/UdpSocketReceiver.h
//#include "UDPSendReceive.h"
#include "SimulatorPrivatePCH.h"
#include "SimUDPReceiver.h"

ASimUDPReceiver::ASimUDPReceiver(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ListenSocket = NULL;
}

void ASimUDPReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//~~~~~~~~~~~~~~~~

	delete UDPReceiver;
	UDPReceiver = nullptr;

	//Clear all sockets!
	//		makes sure repeat plays in Editor dont hold on to old sockets!
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool ASimUDPReceiver::LaunchUDP(const FString name, const FString host, const int32 port)
{
	bool result = StartUDPReceiver(name, host, port);

	return result;
}



//Rama's Start TCP Receiver
bool ASimUDPReceiver::StartUDPReceiver(
	const FString& YourChosenSocketName,
	const FString& TheIP,
	const int32 ThePort
) {

	//ScreenMsg("RECEIVER INIT");

	//~~~

	FIPv4Address Addr;
	FIPv4Address::Parse(TheIP, Addr);

	//Create Socket
	FIPv4Endpoint Endpoint(Addr, ThePort);

	//BUFFER SIZE
	int32 BufferSize = 2 * 1024 * 1024;


	ListenSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);
	;
	if (!ListenSocket)
	{
		return false;
	}
	else
	{
		FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
		UDPReceiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("UDP RECEIVER"));
		UDPReceiver->OnDataReceived().BindUObject(this, &ASimUDPReceiver::Recv);
		UDPReceiver->Start();
		//BPEvent_DataReceived("Hello");
	}

	return true;
}

void ASimUDPReceiver::Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	//ScreenMsg("Received bytes", ArrayReaderPtr->Num());

	//FString Data;
	uint32 PendingSize;
	if (ListenSocket->HasPendingData(PendingSize))
	{
		BPEvent_FStringReceived("Pending", "Hello");
	}
	else
	{
		//int i = 0;
		//Data = "Received";
		FAnyCustomData Data;
		//*ArrayReaderPtr << i;		//Now de-serializing! See AnyCustomData.h
		uint8* dataL = ArrayReaderPtr->GetData();
		int size = ArrayReaderPtr->Num();
		TCHAR * text = (TCHAR*)UTF8_TO_TCHAR(dataL);
		//UInt8ToChar
		char* charString = reinterpret_cast<char*>(dataL);
		std::string typeStd;
		std::string topicStd;
		
		
		int m = 4;
		int g = 0;
		for (int i = m; i < ArrayReaderPtr->Num(); i++)
		{
			if (charString[i] == '\0')
			{
				g++;
				if (g == 2)
				{
					m = i + 3;
					g = 0;
					break;
				}
			}
			else
			{
				typeStd += charString[i];
				g = 0;
			}		
		}
		for (int i = m; i < ArrayReaderPtr->Num(); i++)
		{
			if (charString[i] == '\0')
			{
				g++;
				if (g == 2)
				{
					m = i + 3;
					g = 0;
					break;
				}
			}
			else
			{
				topicStd += charString[i];
				g = 0;
			}
		}
		FString type = FString(typeStd.c_str());
		FString topic = FString(topicStd.c_str());

		if (type == "std_msgs/String")
		{
			StringDatagramRecv(dataL, size, topic,m);
		}
		else if (type == "sim_msgs/BoundingBox")
		{
			BbDatagramRecv(dataL, size, topic, m);
		}
		else if (type == "std_msgs/Float")
		{
			FloatDatagramRecv(dataL, size, topic);
		}
		else
		{
			BbDatagramStringRecv(dataL, size, topic, 4);
		}

	}
}
void ASimUDPReceiver::StringDatagramRecv(uint8* data, int size, FString topic, int startByte)
{	
	FString result = GetFString(data, size, startByte);
	BPEvent_FStringReceived(result, topic);
}
FString ASimUDPReceiver::GetFString(uint8* data, int size, int m)
{
	char* charString = reinterpret_cast<char*>(data);
	std::string dataStd;
	//int size = sizeof(data);
	for (int i = m; i<size; i++)
	{
		if (charString[i] != '\0')
		{
			dataStd += charString[i];
		}
	}
	FString result = FString(dataStd.c_str());
	return result;
}
double ASimUDPReceiver::GetDouble(uint8* data, int size)
{
	int end = size - 1;
	char fill[8];

	fill[0] = data[end];
	fill[1] = data[end - 1];
	fill[2] = data[end - 2];
	fill[3] = data[end - 3];
	fill[4] = data[end - 4];
	fill[5] = data[end - 5];
	fill[6] = data[end - 6];
	fill[7] = data[end - 7];

	double num;
	memcpy(&num, fill, sizeof(double));
	return num;
}
void ASimUDPReceiver::BbDatagramRecv(uint8* data, int size, FString topic, int startByte)
{
	FString result = GetFString(data, size, startByte);
	BPEvent_BbReceived(result, topic);
}
void ASimUDPReceiver::BbDatagramStringRecv(uint8* data, int size, FString topic, int startByte)
{
	FString result = GetFString(data, size, startByte);
	BPEvent_BbReceived(result, topic);
}
void ASimUDPReceiver::FloatDatagramRecv(uint8* data, int size, FString topic)
{
	//Convert to Int
	//int test = (int)(dataL[0] << 24 | dataL[1] << 16 | dataL[2] << 8 | dataL[3]);
	double resultDouble = GetDouble(data, size);
	float result = resultDouble;
	BPEvent_FloatReceived(result, topic);
}
