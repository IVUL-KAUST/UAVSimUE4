#pragma once

#include "ISimulator.h"
#include "Runtime/UMG/Public/UMG.h"

#include "Simulator.generated.h"

USTRUCT()
struct FSimulatorLog
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test")
		FString AxisName = "";
};


class FSimulator : public ISimulator
{
	

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	

		
		

};

