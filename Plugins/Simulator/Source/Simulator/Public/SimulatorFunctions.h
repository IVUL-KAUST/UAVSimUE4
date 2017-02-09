#pragma once
#include "ISimulator.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/Networking/Public/Networking.h"

#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapperModule.h"
#include "Runtime/Engine/Public/HighResScreenshot.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "SimulatorFunctions.generated.h"

USTRUCT()
struct FSimulatorLoger
{
	GENERATED_BODY()


		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test2")
		FString AxisName = "";
};

USTRUCT(BlueprintType)
struct FSimSettingsData
{
	GENERATED_USTRUCT_BODY()

		//Logging
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool enableLogging;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool saveImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool saveGt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool saveUAV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool saveRover;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool saveActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirSaveImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirSaveGt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirSaveUAV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirSaveRover;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirSaveActor;
	//.............
	//Waypoints
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkUavW1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkUavW2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkUavW3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkRoverW1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkRoverW2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkRoverW3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkActorW1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkActorW2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkActorW3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkRoverLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkActorLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirUavW1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirUavW2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirUavW3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirRoverW1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirRoverW2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirRoverW3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirActorW1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirActorW2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirActorW3;
	//........
	//Simulate Logs

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkReplayTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkReplayUav1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkReplayUav2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkReplayUav3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		bool checkReplayUav4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirReplayTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirReplayUav1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirReplayUav2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirReplayUav3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimSettings")
		FText dirReplayUav4;
		
};

USTRUCT(BlueprintType)
struct FTlog
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tlog")
		FVector uavPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tlog")
		FVector uavRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tlog")
		FVector uavVel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tlog")
		FVector targetPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tlog")
		FVector targetRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tlog")
		FVector targetVel;

};
USTRUCT(BlueprintType)
struct FlightLog
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "flightlog")
		FVector uavPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "flightlog")
		FVector uavRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "flightlog")
		FVector uavVel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "flightlog")
		float leftStickX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "flightlog")
		float leftStickY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "flightlog")
		float rightStickX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "flightlog")
		float rightStickY;
};

USTRUCT(BlueprintType)
struct FPidData
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID")
		float lastTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID")
		float integral;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID")
		float prevError;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID")
		float output;
};
USTRUCT(BlueprintType)
struct FBoundingBox
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
		FVector2D center;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
		FString result;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
		FVector extents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
		int32 yMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
		int32 xMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
		int32 xMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BB")
		int32 yMax;

};




UCLASS()
class USimulatorFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	    USimulatorFunctions(const FObjectInitializer& ObjectInitializer);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static bool CaptureComponent2D_SaveImage(class USceneCaptureComponent2D* Target, const FString ImagePath, const FLinearColor ClearColour);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static FPidData pidLoop(float error, float curTime, float Kp, float Ki, float Kd, FPidData pidData);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static FBoundingBox CaptureComponent2D_BBImage(class USceneCaptureComponent2D* Target, const FString ImagePath, const FLinearColor ClearColour);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static FVector2D bb_getCenter(FBoundingBox bb);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static FVector bb_getExtents(FBoundingBox bb);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static bool saveTrackingLog(const FString FilePath, FString frame, FVector uavPos, FVector targetPos, FVector uavRot, FVector targetRot, FVector uavVel, FVector targetVel);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static bool saveFlightLog(const FString FilePath, FString frame, FVector uavPos, FVector uavRot, FVector uavVel, float leftStickX, float leftStickY, float rightStickX, float rightStickY);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static bool saveWayPoints(const FString FilePath, TArray<FVector>  wayPoints);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static bool convertLogToWayPoints(const FString FilePath, const FString FilePathOut, float wayPointDistance);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static FString readBoolTextfile(const FString FilePath);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static bool saveBoolTextfile(const FString FilePath, FString text);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static FBoundingBox readBBTextfile(const FString FilePath);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static FBoundingBox readBBFString(const FString result);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static TArray<FString> getDirList(const FString FilePath, FString Ext);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static FTlog readLogfile(const FString FilePath);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static FlightLog readFlightLogfile(const FString FilePath);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static TArray<FTlog> readLogDir(const FString FilePath);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static TArray<FlightLog> readFlightLogDir(const FString FilePath);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static TArray<FVector> readWayPoints(const FString FilePath);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static bool saveBBTextfile(const FString FilePath, FString frame, FBoundingBox bb);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static bool sendBBTextfile(FString TheIP, int32 ThePort, FString frame, FBoundingBox bb);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static FSimSettingsData loadSimSetings();

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static bool saveSimSetings(FSimSettingsData simData);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static bool sendDatagram(class USceneCaptureComponent2D* Target);

		UFUNCTION(Category = "Simulator|SceneCapture", BlueprintCallable)
			static bool sendStringDatagram(FString TheIP, int32 ThePort, FString data);
};
