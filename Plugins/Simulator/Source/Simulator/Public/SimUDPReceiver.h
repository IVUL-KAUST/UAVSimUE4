#pragma once
/*

By Rama

*/
#include "ISimulator.h"
//Networking
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/Networking/Public/Networking.h"
#include <string>
#include <fstream>

//Base
#include "SimUDPReceiver.generated.h"

USTRUCT(BlueprintType)
struct FAnyCustomData
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulator|SceneCapture")
		int Name;

	FAnyCustomData()
	{}
};

FORCEINLINE FArchive& operator<<(FArchive &Ar, FAnyCustomData& TheStruct)
{
	Ar << TheStruct.Name;
	//Ar << TheStruct.Count;
	//Ar << TheStruct.Scale;
	//Ar << TheStruct.Color;

	return Ar;
}

UCLASS()
class ASimUDPReceiver : public AActor
{
	GENERATED_UCLASS_BODY()

		//====================================================
		//		Data Received Events!
public:
	/** Data has been received!! */
	UFUNCTION(BlueprintImplementableEvent)
		void BPEvent_FStringReceived(const FString& ReceivedData, const FString& Topic);

	UFUNCTION(BlueprintImplementableEvent)
		void BPEvent_BbReceived(const FString& ReceivedData, const FString& Topic);

	UFUNCTION(BlueprintImplementableEvent)
		void BPEvent_FloatReceived(const float& ReceivedData, const FString& Topic);

	UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
		bool LaunchUDP(const FString name, const FString host, const int32 port);

	//====================================================

public:
	FSocket* ListenSocket;
	//uint8 * Data;

	FUdpSocketReceiver* UDPReceiver = nullptr;
	void Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);

	void StringDatagramRecv(uint8* data, int size, FString topic, int startByte);
	void BbDatagramRecv(uint8* data,int size, FString topic, int startByte);
	void BbDatagramStringRecv(uint8* data, int size, FString topic, int startByte);
	void FloatDatagramRecv(uint8* data, int size, FString topic);
	FString GetFString(uint8* data,int size, int m);
	double GetDouble(uint8* data, int size);
	
	bool StartUDPReceiver(
		const FString& YourChosenSocketName,
		const FString& TheIP,
		const int32 ThePort
	);

public:

	/** Called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};