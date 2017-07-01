/*
/ Copyright 2016-2017 King Abdullah University of Science and Technology. All Rights Reserved.
Authors: Matthias Mueller, Neil Smith and Bernard Ghanem
For Citation: https://ivul.kaust.edu.sa/Pages/pub-benchmark-simulator-uav.aspx
License: See License.txt

*/

#include "SimulatorPrivatePCH.h"

#include "SimulatorFunctions.h"




USimulatorFunctions::USimulatorFunctions(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool USimulatorFunctions::CaptureComponent2D_SaveImage(class USceneCaptureComponent2D* Target, const FString ImagePath, const FLinearColor ClearColour)
{

	if ((Target == nullptr) || (Target->TextureTarget == nullptr))
	{
		return false;
	}

	FRenderTarget* RenderTarget = Target->TextureTarget->GameThread_GetRenderTargetResource();
	if (RenderTarget == nullptr)
	{
		return false;
	}

	TArray<FColor> RawPixels;
	

	// Format not supported - use PF_B8G8R8A8.
	if (Target->TextureTarget->GetFormat() != PF_B8G8R8A8)
	{
		// TRACEWARN("Format not supported - use PF_B8G8R8A8.");
		return false;
	}

	if (!RenderTarget->ReadPixels(RawPixels))
	{
		return false;
	}

	// Convert to FColor.
	FColor ClearFColour = ClearColour.ToFColor(false); // FIXME - want sRGB or not?

	for (auto& Pixel : RawPixels)
	{
		// Switch Red/Blue changes.
		const uint8 PR = Pixel.R;
		const uint8 PB = Pixel.B;
		Pixel.R = PB;
		Pixel.B = PR;

		// Set alpha based on RGB values of ClearColour.
		Pixel.A = ((Pixel.R == ClearFColour.R) && (Pixel.R == ClearFColour.R) && (Pixel.R == ClearFColour.R)) ? 0 : 255;
		

	}
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	

	const int32 Width = Target->TextureTarget->SizeX;
	const int32 Height = Target->TextureTarget->SizeY;

	if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&RawPixels[0], RawPixels.Num() * sizeof(FColor), Width, Height, ERGBFormat::RGBA, 8))
	{
		FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(), *ImagePath);
		return true;
	}

	return false;


}

FPidData USimulatorFunctions::pidLoop(float error, float curTime, float Kp, float Ki, float Kd, FPidData pidData)
{

	float lastTime = pidData.lastTime;
	float integral = pidData.integral;
	float prevError = pidData.prevError;

	float dt = curTime - lastTime;
	lastTime = curTime;
	integral = integral + error*dt;
	float derviative = (error - prevError) / dt;
	float output = Kp * error + Ki * integral + Kd*derviative;
	prevError = error;

	pidData.lastTime = lastTime;
	pidData.integral = integral;
	pidData.prevError = prevError;
	pidData.output = output;

	return pidData;
}

FVector2D USimulatorFunctions::bb_getCenter(FBoundingBox bb)
{
	return bb.center;
}
FVector USimulatorFunctions::bb_getExtents(FBoundingBox bb)
{
	return bb.extents;
}
FBoundingBox USimulatorFunctions::CaptureComponent2D_BBImage(class USceneCaptureComponent2D* Target, const FString ImagePath, const FLinearColor ClearColour)
{
	FBoundingBox bb;

	//VInput.Key = KeyEvent.GetKey();
	//VInput.KeyAsString

	// Bad scene capture component! No render target! Stay! Stay! Ok, feed!... wait, where was I?
	if ((Target == nullptr) || (Target->TextureTarget == nullptr))
	{
		return bb;
	}

	FRenderTarget* RenderTarget = Target->TextureTarget->GameThread_GetRenderTargetResource();
	if (RenderTarget == nullptr)
	{
		return bb;
	}

	TArray<FColor> RawPixels;

	// Format not supported - use PF_B8G8R8A8.
	if (Target->TextureTarget->GetFormat() != PF_B8G8R8A8)
	{
		// TRACEWARN("Format not supported - use PF_B8G8R8A8.");
		//return false;
	}

	if (!RenderTarget->ReadPixels(RawPixels))
	{
		return bb;
	}

	// Convert to FColor.
	FColor ClearFColour = ClearColour.ToFColor(false); // FIXME - want sRGB or not?
	float count = 0;
	int32 xpix = 0;
	int32 ypix = 0;
	int32 left = -1;
	int32 right = -1;
	int32 top = -1;
	int32 bottom = -1;

	FVector2D center;
	const int32 Width = Target->TextureTarget->SizeX;
	const int32 Height = Target->TextureTarget->SizeY;

	for (auto& Pixel : RawPixels)
	{

		if (xpix == Width)
		{
			xpix = 0;
			ypix++;
		}
		if (Pixel.G > 0 && top == -1)
		{

			top = ypix;
			left = xpix;
			right = xpix;
			bottom = ypix;
		}
		if (Pixel.G > 0 && top != -1 && xpix < left)
		{
			left = xpix;

		}
		if (Pixel.G > 0 && top != -1 && xpix > right)
		{
			right = xpix;

		}
		if (Pixel.G > 0 && top != -1 && ypix > bottom)
		{
			bottom = ypix;
		}
		// Switch Red/Blue changes.
		const uint8 PR = Pixel.R;
		const uint8 PB = Pixel.B;
		Pixel.R = PB;
		Pixel.B = PR;

		// Set alpha based on RGB values of ClearColour.
		Pixel.A = ((Pixel.R == ClearFColour.R) && (Pixel.R == ClearFColour.R) && (Pixel.R == ClearFColour.R)) ? 0 : 255;
		count = count + 1;
		xpix++;
	}


	FVector extents;
	extents.X = left;
	extents.Y = right;
	extents.Z = bottom;
	bb.extents = extents;
	bb.xMin = left;
	bb.xMax = right;
	bb.yMin = top;
	bb.yMax = bottom;

	center.X = (bb.xMax - bb.xMin) / 2 + bb.xMin;
	center.Y = (bb.yMax - bb.yMin) / 2 + bb.yMin;
	bb.center = center;
	return bb;


}
bool USimulatorFunctions::saveTrackingLog(const FString FilePath, FString frame, FVector uavPos, FVector targetPos, FVector uavRot, FVector targetRot, FVector uavVel, FVector targetVel)
{

	frame.Append("," + FString::SanitizeFloat(uavPos.X) + "," + FString::SanitizeFloat(uavPos.Y) + "," + FString::SanitizeFloat(uavPos.Z));
	frame.Append("," + FString::SanitizeFloat(uavRot.X) + "," + FString::SanitizeFloat(uavRot.Y) + "," + FString::SanitizeFloat(uavRot.Z));
	frame.Append("," + FString::SanitizeFloat(uavVel.X) + "," + FString::SanitizeFloat(uavVel.Y) + "," + FString::SanitizeFloat(uavVel.Z));

	frame.Append("," + FString::SanitizeFloat(targetPos.X) + "," + FString::SanitizeFloat(targetPos.Y) + "," + FString::SanitizeFloat(targetPos.Z));
	frame.Append("," + FString::SanitizeFloat(targetRot.X) + "," + FString::SanitizeFloat(targetRot.Y) + "," + FString::SanitizeFloat(targetRot.Z));
	frame.Append("," + FString::SanitizeFloat(targetVel.X) + "," + FString::SanitizeFloat(targetVel.Y) + "," + FString::SanitizeFloat(targetVel.Z));

	FFileHelper::SaveStringToFile(frame, *FilePath);
	return true;
}
bool USimulatorFunctions::saveFlightLog(const FString FilePath, FString frame, FVector uavPos, FVector uavRot, FVector uavVel, float leftStickX, float leftStickY, float rightStickX, float rightStickY)
{

	frame.Append("," + FString::SanitizeFloat(uavPos.X) + "," + FString::SanitizeFloat(uavPos.Y) + "," + FString::SanitizeFloat(uavPos.Z));
	frame.Append("," + FString::SanitizeFloat(uavRot.X) + "," + FString::SanitizeFloat(uavRot.Y) + "," + FString::SanitizeFloat(uavRot.Z));
	frame.Append("," + FString::SanitizeFloat(uavVel.X) + "," + FString::SanitizeFloat(uavVel.Y) + "," + FString::SanitizeFloat(uavVel.Z));

	frame.Append("," + FString::SanitizeFloat(leftStickX) + "," + FString::SanitizeFloat(leftStickY));
	frame.Append("," + FString::SanitizeFloat(rightStickX) + "," + FString::SanitizeFloat(rightStickY));
	
	FFileHelper::SaveStringToFile(frame, *FilePath);
	return true;
}
TArray<FTlog> USimulatorFunctions::readLogDir(const FString FilePath)
{
	TArray<FTlog> logs;
	TArray<FString> files;
	FString Ext = "log";
	files = getDirList(FilePath, Ext);
	for (int i = 0; i < files.Num(); i++)
	{
		FTlog tlog;
		FString fileName = FilePath + "/" + files[i];
		tlog = readLogfile(fileName);
		logs.Add(tlog);
	}
	return logs;

}
TArray<FlightLog> USimulatorFunctions::readFlightLogDir(const FString FilePath)
{
	TArray<FlightLog> logs;
	TArray<FString> files;
	FString Ext = "log";
	files = getDirList(FilePath, Ext);
	for (int i = 0; i < files.Num(); i++)
	{
		FlightLog tlog;
		FString fileName = FilePath + "/" + files[i];
		tlog = readFlightLogfile(fileName);
		logs.Add(tlog);
	}
	return logs;

}
TArray<FVector> USimulatorFunctions::readWayPoints(const FString FilePath)
{
	TArray<FVector> wayPoints;
	
	FString result = "";
	FFileHelper::LoadFileToString(result, *FilePath);
	TArray<FString> stringData;
	const FString newLine = "\n";
	int32 num = 0;
	num = result.ParseIntoArray(stringData, *newLine);
	for (int i = 0; i < stringData.Num(); i++)
	{
		FVector dataVector;
		TArray<FString> stringVector;
		const FString comma = ",";
		int32 num2 = 0;
		num2 = stringData[i].ParseIntoArray(stringVector, *comma);

		dataVector.X = FCString::Atof(*stringVector[0]);
		dataVector.Y = FCString::Atof(*stringVector[1]);
		dataVector.Z = FCString::Atof(*stringVector[2]);
		
		
		wayPoints.Add(dataVector);
	}
	return wayPoints;

}
bool USimulatorFunctions::convertLogToWayPoints(const FString FilePath, const FString FilePathOut, float wayPointDistance)
{
	TArray<FVector>  wayPoints;

	TArray<FlightLog> logs;
	logs = readFlightLogDir(FilePath);

	float currentDistance = 0;
	for (int i = 0; i < logs.Num(); i++)
	{
		if (i == logs.Num() - 1) //Create WayPoint At End
		{
			wayPoints.Add(logs[i].uavPos);
		}
		else
		{
			FlightLog begin = logs[i];
			FlightLog end = logs[i + 1];
			float distance = (end.uavPos - begin.uavPos).Size();
			currentDistance += distance;
			if (currentDistance > wayPointDistance)
			{
				currentDistance = 0;
				wayPoints.Add(end.uavPos);
			}

		}
	}

	if (wayPoints.Num() > 0)
	{
		saveWayPoints(FilePathOut, wayPoints);
	}
	else
	{
		return false;
	}

	return true;
}
TArray<FString> USimulatorFunctions::getDirList(const FString FilePath, FString Ext)
{
	TArray<FString> files;

	//FPaths::NormalizeDirectoryName(FilePath);

	IFileManager& FileManager = IFileManager::Get();

	if (Ext == "")
	{
		Ext = "*.*";
	}
	else
	{
		Ext = (Ext.Left(1) == ".") ? "*" + Ext : "*." + Ext;
	}
	FString FinalPath = FilePath + "/" + Ext;
	FileManager.FindFiles(files, *FinalPath, true, false);

	return files;
}
FTlog USimulatorFunctions::readLogfile(const FString FilePath)
{
	FTlog tlog;
	FString result = "";
	FFileHelper::LoadFileToString(result, *FilePath);
	TArray<FString> stringData;
	const FString comma = ",";
	int32 num = 0;
	num = result.ParseIntoArray(stringData, *comma);
	if (num < 16)
	{
		return tlog;
	}
	else
	{
		FVector uavPos;
		FVector uavRot;
		FVector uavVel;
		FVector targetPos;
		FVector targetRot;
		FVector targetVel;

		uavPos.X = FCString::Atof(*stringData[1]);
		uavPos.Y = FCString::Atof(*stringData[2]);
		uavPos.Z = FCString::Atof(*stringData[3]);
		uavRot.X = FCString::Atof(*stringData[4]);
		uavRot.Y = FCString::Atof(*stringData[5]);
		uavRot.Z = FCString::Atof(*stringData[6]);
		uavVel.X = FCString::Atof(*stringData[7]);
		uavVel.Y = FCString::Atof(*stringData[8]);
		uavVel.Z = FCString::Atof(*stringData[9]);

		targetPos.X = FCString::Atof(*stringData[10]);
		targetPos.Y = FCString::Atof(*stringData[11]);
		targetPos.Z = FCString::Atof(*stringData[12]);
		targetRot.X = FCString::Atof(*stringData[13]);
		targetRot.Y = FCString::Atof(*stringData[14]);
		targetRot.Z = FCString::Atof(*stringData[15]);
		targetVel.X = FCString::Atof(*stringData[16]);
		targetVel.Y = FCString::Atof(*stringData[17]);
		targetVel.Z = FCString::Atof(*stringData[18]);

		tlog.uavPos = uavPos;
		tlog.uavRot = uavRot;
		tlog.uavVel = uavVel;

		tlog.targetPos = targetPos;
		tlog.targetRot = targetRot;
		tlog.targetVel = targetVel;
	}
	return tlog;
}
FlightLog USimulatorFunctions::readFlightLogfile(const FString FilePath)
{
	FlightLog tlog;
	FString result = "";
	FFileHelper::LoadFileToString(result, *FilePath);
	TArray<FString> stringData;
	const FString comma = ",";
	int32 num = 0;
	num = result.ParseIntoArray(stringData, *comma);
	if (num < 13)
	{
		return tlog;
	}
	else
	{
		FVector uavPos;
		FVector uavRot;
		FVector uavVel;
		float leftStickX;
		float leftStickY;
		float rightStickX;
		float rightStickY;

		uavPos.X = FCString::Atof(*stringData[1]);
		uavPos.Y = FCString::Atof(*stringData[2]);
		uavPos.Z = FCString::Atof(*stringData[3]);
		uavRot.X = FCString::Atof(*stringData[4]);
		uavRot.Y = FCString::Atof(*stringData[5]);
		uavRot.Z = FCString::Atof(*stringData[6]);
		uavVel.X = FCString::Atof(*stringData[7]);
		uavVel.Y = FCString::Atof(*stringData[8]);
		uavVel.Z = FCString::Atof(*stringData[9]);

		leftStickX = FCString::Atof(*stringData[10]);
		leftStickY = FCString::Atof(*stringData[11]);
		rightStickX = FCString::Atof(*stringData[12]);
		rightStickY = FCString::Atof(*stringData[13]);

		tlog.uavPos = uavPos;
		tlog.uavRot = uavRot;
		tlog.uavVel = uavVel;

		tlog.leftStickX = leftStickX;
		tlog.leftStickY = leftStickY;
		tlog.rightStickX = rightStickX;
		tlog.rightStickY = rightStickY;

	}
	return tlog;
}

FString USimulatorFunctions::readBoolTextfile(const FString FilePath)
{
	FString result = "";
	FFileHelper::LoadFileToString(result, *FilePath);

	return result;
}
bool USimulatorFunctions::saveBoolTextfile(const FString FilePath, FString text)
{
	FFileHelper::SaveStringToFile(text, *FilePath);
	return true;
}

FBoundingBox USimulatorFunctions::readBBTextfile(const FString FilePath)
{
	FString result = "";
	FFileHelper::LoadFileToString(result, *FilePath);

	TArray<FString> stringData;
	const FString comma = ",";
	int32 num = 0;
	num = result.ParseIntoArray(stringData, *comma);

	FBoundingBox bb;
	FVector2D center;
	FVector extents;
	int32 x;
	int32 y;
	int32 w;
	int32 h;
	if (num < 4)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
	}
	else
	{


		x = FCString::Atoi(*stringData[0]);
		y = FCString::Atoi(*stringData[1]);
		w = FCString::Atoi(*stringData[2]);
		h = FCString::Atoi(*stringData[3]);
	}


	extents.X = x;
	extents.Y = x + w;
	extents.Z = y + h;
	bb.extents = extents;
	bb.xMin = x;
	bb.xMax = x + w;
	bb.yMin = y;
	bb.yMax = y + h;

	center.X = (bb.xMax - bb.xMin) / 2 + bb.xMin;
	center.Y = (bb.yMax - bb.yMin) / 2 + bb.yMin;
	bb.center = center;

	//	bb.xMax = stringData.Max();
	//bb.yMax = stringData.Num();
	//	bb.xMin = num;
	bb.result = result;
	return bb;
}
FBoundingBox USimulatorFunctions::readBBFString(const FString result)
{
	//FString result = "";
	//FFileHelper::LoadFileToString(result, *FilePath);

	TArray<FString> stringData;
	const FString comma = ",";
	int32 num = 0;
	num = result.ParseIntoArray(stringData, *comma);

	FBoundingBox bb;
	FVector2D center;
	FVector extents;
	int32 x;
	int32 y;
	int32 w;
	int32 h;
	if (num < 4)
	{
		x = 0;
		y = 0;
		w = 0;
		h = 0;
	}
	else
	{


		x = FCString::Atoi(*stringData[0]);
		y = FCString::Atoi(*stringData[1]);
		w = FCString::Atoi(*stringData[2]);
		h = FCString::Atoi(*stringData[3]);
	}


	extents.X = x;
	extents.Y = x + w;
	extents.Z = y + h;
	bb.extents = extents;
	bb.xMin = x;
	bb.xMax = x + w;
	bb.yMin = y;
	bb.yMax = y + h;

	center.X = (bb.xMax - bb.xMin) / 2 + bb.xMin;
	center.Y = (bb.yMax - bb.yMin) / 2 + bb.yMin;
	bb.center = center;

	//	bb.xMax = stringData.Max();
	//bb.yMax = stringData.Num();
	//	bb.xMin = num;
	bb.result = result;
	return bb;
}
bool USimulatorFunctions::saveBBTextfile(const FString FilePath, FString frame, FBoundingBox bb)
{

	int32 width = bb.xMax - bb.xMin;
	int32 height = bb.yMax - bb.yMin;
	FString text = "";
	text.Append(FString::SanitizeFloat(bb.xMin) + "," + FString::SanitizeFloat(bb.yMin) + "," + FString::SanitizeFloat(width) + "," + FString::SanitizeFloat(height));

	FFileHelper::SaveStringToFile(text, *FilePath);
	return true;
}
bool USimulatorFunctions::sendBBTextfile(FString TheIP, int32 ThePort, FString frame, FBoundingBox bb)
{
	int32 width = bb.xMax - bb.xMin;
	int32 height = bb.yMax - bb.yMin;
	FString text = "";
	text.Append(FString::SanitizeFloat(bb.xMin) + "," + FString::SanitizeFloat(bb.yMin) + "," + FString::SanitizeFloat(width) + "," + FString::SanitizeFloat(height));
	sendStringDatagram(TheIP, ThePort, text);
	return true;
}
bool USimulatorFunctions::saveWayPoints(const FString FilePath, TArray<FVector> wayPoints)
{
	FString text = "";
	
	for (int i = 0; i < wayPoints.Num(); i++)
	{
		if (i != 0)
		{
			text.Append("\n");
		}

			text.Append(FString::SanitizeFloat(wayPoints[i].X) + "," + FString::SanitizeFloat(wayPoints[i].Y) + "," + FString::SanitizeFloat(wayPoints[i].Z));
		
	}
	FFileHelper::SaveStringToFile(text, *FilePath);
	return true;
}

FSimSettingsData USimulatorFunctions::loadSimSetings()
{
	int32 pos = 0;
	FPaths::GetProjectFilePath().FindLastChar('/', pos);
	FString settingsPath = FPaths::GetProjectFilePath().Left(pos);
	settingsPath.Append("/Config/SimSettings.ini");

	FSimSettingsData simData;

	FString result = "";
	FFileHelper::LoadFileToString(result, *settingsPath);
	TArray<FString> stringData;
	const FString newLine = "\n";
	int32 num = 0;
	num = result.ParseIntoArray(stringData, *newLine,false);

	TArray<FString> values;
	
	for (int i = 0; i < stringData.Num(); i++)
	{
		TArray<FString> valueData;
		const FString equal = "=";
		stringData[i].ParseIntoArray(valueData, *equal,false);
		if (valueData.Num() > 1)
		{
			values.Push(valueData[1].TrimTrailing());

			//values.Push("");
		}
		else
		{
			values.Push("");
		}

	}
	
	//Load into Struct
	if (values.Num() == 41)
	{
		simData.enableLogging = (values[0] == "1") ? true : false;
		simData.saveImage = (values[1] == "1") ? true : false;
		simData.saveGt = (values[2] == "1") ? true : false;
		simData.saveUAV = (values[3] == "1") ? true : false;
		simData.saveRover = (values[4] == "1") ? true : false;
		simData.saveActor = (values[5] == "1") ? true : false;

		simData.dirSaveImage = FText::FromString(values[6]);
		simData.dirSaveGt = FText::FromString(values[7]);
		simData.dirSaveUAV = FText::FromString(values[8]);
		simData.dirSaveRover = FText::FromString(values[9]);
		simData.dirSaveActor = FText::FromString(values[10]);

		simData.checkUavW1 = (values[11] == "1") ? true : false;
		simData.checkUavW2 = (values[12] == "1") ? true : false;
		simData.checkUavW3 = (values[13] == "1") ? true : false;

		simData.checkRoverW1 = (values[14] == "1") ? true : false;
		simData.checkRoverW2 = (values[15] == "1") ? true : false;
		simData.checkRoverW3 = (values[16] == "1") ? true : false;

		simData.checkActorW1 = (values[17] == "1") ? true : false;
		simData.checkActorW2 = (values[18] == "1") ? true : false;
		simData.checkActorW3 = (values[19] == "1") ? true : false;

		simData.checkRoverLoop = (values[20] == "1") ? true : false;
		simData.checkActorLoop = (values[21] == "1") ? true : false;

		simData.dirUavW1 = FText::FromString(values[22]);
		simData.dirUavW2 = FText::FromString(values[23]);
		simData.dirUavW3 = FText::FromString(values[24]);

		simData.dirRoverW1 = FText::FromString(values[25]);
		simData.dirRoverW2 = FText::FromString(values[26]);
		simData.dirRoverW3 = FText::FromString(values[27]);

		simData.dirActorW1 = FText::FromString(values[28]);
		simData.dirActorW2 = FText::FromString(values[29]);
		simData.dirActorW3 = FText::FromString(values[30]);

		simData.checkReplayTarget = (values[31] == "1") ? true : false;
		simData.checkReplayUav1 = (values[32] == "1") ? true : false;
		simData.checkReplayUav2 = (values[33] == "1") ? true : false;
		simData.checkReplayUav3 = (values[34] == "1") ? true : false;
		simData.checkReplayUav4 = (values[35] == "1") ? true : false;

		simData.dirReplayTarget = FText::FromString(values[36]);
		simData.dirReplayUav1 = FText::FromString(values[37]);
		simData.dirReplayUav2 = FText::FromString(values[38]);
		simData.dirReplayUav3 = FText::FromString(values[39]);
		simData.dirReplayUav4 = FText::FromString(values[40]);
	}
	return simData;
}
bool USimulatorFunctions::sendStringDatagram(FString TheIP, int32 ThePort, FString data)
{
	TSharedPtr<FInternetAddr>	RemoteAddr;
	FSocket* SenderSocket;


	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bIsValid;
	FString YourChosenSocketName = "TX_BB";
	TheIP = "127.0.0.1";
	ThePort = 25001;
	RemoteAddr->SetIp(*TheIP, bIsValid);
	RemoteAddr->SetPort(ThePort);

	if (!bIsValid)
	{
		return false;
	}

	SenderSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsReusable()
		.WithBroadcast()
		;
	int32 BytesSent = 0;

	FString serialized = data;
	//serialized = FString::FromInt(sizeD) + " " + FString::FromInt(alSize) + " " + FString::FromInt(numSize) + " " + FString::FromInt(uSize);
	TCHAR *serializedChar = serialized.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);
	int32 sent = 0;
	uint8* dataGram = (uint8*)TCHAR_TO_UTF8(serializedChar);
	
	SenderSocket->SendTo(dataGram, size, BytesSent, *RemoteAddr);

	SenderSocket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);

	return true;

}

bool USimulatorFunctions::sendDatagram(class USceneCaptureComponent2D* Target)
{
	TSharedPtr<FInternetAddr>	RemoteAddr;
	FSocket* SenderSocket;
	

	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bIsValid;
	FString YourChosenSocketName = "TX_Image";
	FString TheIP = "127.0.0.1";
	int32 ThePort = 25000;
	RemoteAddr->SetIp(*TheIP, bIsValid);
	RemoteAddr->SetPort(ThePort);

	if (!bIsValid)
	{
		return false;
	}

	SenderSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsReusable()
		.WithBroadcast()
		;
	int32 BytesSent = 0;

	if (true)
	{
		
		if ((Target == nullptr) || (Target->TextureTarget == nullptr))
		{
			return false;
		}

		FRenderTarget* RenderTarget = Target->TextureTarget->GameThread_GetRenderTargetResource();
		if (RenderTarget == nullptr)
		{
			return false;
		}

		TArray<FColor> RawPixels;
		if (!RenderTarget->ReadPixels(RawPixels))
		{
			return false;
		}
		int32 sizeD = RawPixels.Num() * sizeof(FColor);

		if (true)
		{
			
			const int32 Width = Target->TextureTarget->SizeX;
			const int32 Height = Target->TextureTarget->SizeY;
			IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
			IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
			int32 alSize =0;
			int32 numSize =0;
			//int32 uSize = 0;
			TArray<uint8> dataIm;
			
			if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&RawPixels[0], RawPixels.Num() * sizeof(FColor), Width, Height, ERGBFormat::BGRA, 8))
			{
				//FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(), *ImagePath);
				dataIm = ImageWrapper->GetCompressed(50);
				alSize = dataIm.GetAllocatedSize();
				numSize = dataIm.Num();
				//uSize = dataIm.Num() * sizeof(uint8);
			}
			
		    SenderSocket->SendTo(dataIm.GetData(), dataIm.Num(), BytesSent, *RemoteAddr);

			SenderSocket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);
			
		}
		
	}
	
	return true;
}

bool USimulatorFunctions::saveSimSetings(FSimSettingsData simData)
{
	int32 pos = 0;
	FPaths::GetProjectFilePath().FindLastChar('/', pos);
	FString settingsPath = FPaths::GetProjectFilePath().Left(pos);
	settingsPath.Append("/Config/SimSettings.ini");
	FString result = "";
	result.Append("enableLogging");
	result.Append((simData.enableLogging) ? "=1\n" : "=0\n");
	result.Append("saveImage");
	result.Append((simData.saveImage) ? "=1\n" : "=0\n");
	result.Append("saveGt");
	result.Append((simData.saveGt) ? "=1\n" : "=0\n");
	result.Append("saveUAV");
	result.Append((simData.saveUAV) ? "=1\n" : "=0\n");
	result.Append("saveRover");
	result.Append((simData.saveRover) ? "=1\n" : "=0\n");
	result.Append("saveActor");
	result.Append((simData.saveActor) ? "=1\n" : "=0\n");

	result.Append("dirSaveImage");
	result.Append("=" + simData.dirSaveImage.ToString() + "\n");
	result.Append("dirSaveGt");
	result.Append("=" + simData.dirSaveGt.ToString() + "\n");
	result.Append("dirSaveUAV");
	result.Append("=" + simData.dirSaveUAV.ToString() + "\n");
	result.Append("dirSaveRover");
	result.Append("=" + simData.dirSaveRover.ToString() + "\n");
	result.Append("dirSaveActor");
	result.Append("=" + simData.dirSaveActor.ToString() + "\n");

	result.Append("checkUavW1");
	result.Append((simData.checkUavW1) ? "=1\n" : "=0\n");
	result.Append("checkUavW2");
	result.Append((simData.checkUavW2) ? "=1\n" : "=0\n");
	result.Append("checkUavW3");
	result.Append((simData.checkUavW3) ? "=1\n" : "=0\n");

	result.Append("checkRoverW1");
	result.Append((simData.checkRoverW1) ? "=1\n" : "=0\n");
	result.Append("checkRoverW2");
	result.Append((simData.checkRoverW2) ? "=1\n" : "=0\n");
	result.Append("checkRoverW3");
	result.Append((simData.checkRoverW3) ? "=1\n" : "=0\n");

	result.Append("checkActorW1");
	result.Append((simData.checkActorW1) ? "=1\n" : "=0\n");
	result.Append("checkActorW2");
	result.Append((simData.checkActorW2) ? "=1\n" : "=0\n");
	result.Append("checkActorW3");
	result.Append((simData.checkActorW3) ? "=1\n" : "=0\n");

	result.Append("checkRoverLoop");
	result.Append((simData.checkRoverLoop) ? "=1\n" : "=0\n");
	result.Append("checkActorLoop");
	result.Append((simData.checkActorLoop) ? "=1\n" : "=0\n");

	result.Append("dirUavW1");
	result.Append("=" + simData.dirUavW1.ToString() + "\n");
	result.Append("dirUavW2");
	result.Append("=" + simData.dirUavW2.ToString() + "\n");
	result.Append("dirUavW3");
	result.Append("=" + simData.dirUavW3.ToString() + "\n");

	result.Append("dirRoverW1");
	result.Append("=" + simData.dirRoverW1.ToString() + "\n");
	result.Append("dirRoverW2");
	result.Append("=" + simData.dirRoverW2.ToString() + "\n");
	result.Append("dirRoverW3");
	result.Append("=" + simData.dirRoverW3.ToString() + "\n");

	result.Append("dirActorW1");
	result.Append("=" + simData.dirActorW1.ToString() + "\n");
	result.Append("dirActorW2");
	result.Append("=" + simData.dirActorW2.ToString() + "\n");
	result.Append("dirActorW3");
	result.Append("=" + simData.dirActorW3.ToString() + "\n");

	result.Append("checkReplayTarget");
	result.Append((simData.checkReplayTarget) ? "=1\n" : "=0\n");
	result.Append("checkReplayUav1");
	result.Append((simData.checkReplayUav1) ? "=1\n" : "=0\n");
	result.Append("checkReplayUav2");
	result.Append((simData.checkReplayUav2) ? "=1\n" : "=0\n");
	result.Append("checkReplayUav3");
	result.Append((simData.checkReplayUav3) ? "=1\n" : "=0\n");
	result.Append("checkReplayUav4");
	result.Append((simData.checkReplayUav4) ? "=1\n" : "=0\n");

	result.Append("dirReplayTarget");
	result.Append("=" + simData.dirReplayTarget.ToString() + "\n");
	result.Append("dirReplayUav1");
	result.Append("=" + simData.dirReplayUav1.ToString() + "\n");
	result.Append("dirReplayUav2");
	result.Append("=" + simData.dirReplayUav2.ToString() + "\n");
	result.Append("dirReplayUav3");
	result.Append("=" + simData.dirReplayUav3.ToString() + "\n");
	result.Append("dirReplayUav4");
	result.Append("=" + simData.dirReplayUav4.ToString());

	FFileHelper::SaveStringToFile(result, *settingsPath);
	return true;
}
