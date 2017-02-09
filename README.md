# UE4 UAV Flight Simulator

Version: 1.01 (January 29th, 2017)

Current Supporting UE4 Engine: 4.14.3

Authors: Matthias Mueller, Neil Smith and Bernard Ghanem

For Citation: https://ivul.kaust.edu.sa/Pages/pub-benchmark-simulator-uav.aspx

License: See LICENSE file

Contributors: Special thanks to Rama for inspiration from VictoryPlugin. The Car is based off Peter L. Newton's Car AI (https://forums.unrealengine.com/showthread.php?25073-Community-A-I-Project-(Dev-Thread)-Bot-Car-and-soon-Flying-AI). The current level is from the startere content of UE4. The S1000 is adapted for UE4 from Google 3D Warehouse by Torsten L (https://3dwarehouse.sketchup.com/model.html?id=u2dd74708-c644-4e69-925d-9c60d9c51aec)

## Introduction

The UE4 flight simulator provides a set of assets, blueprints, and C++ classes for simulating the flight of a Multirotor copter in a photo-realistic environment. Unique to the simulator is the ability to record frames of the rendered environment and groundtruth tracked object by the UAV. Multiple instances of the UAV can be dropped into the environment and the blueprint can be modified to record multiple cameras. In addition, blueprints are provided for simulating trajectories of a physic based vehicle and navmesh controlled human.

This current github version uses the starter level from the UE4 launcher. The content unique to the simulator is contained within game/Simulator and can be migrated into any project.

*Note that this version fully integrates the Vive HMD and Controller but is not required to play in editor/viewport.

## Install Instructions

-Current version is compatible for UE4 Release (Please install from Epic Launcher...See top of Readme for Supported Version)

-Clone project into folder

-Launch UAVSimUE4.uproject

-Note that on first run the Loading... will get to 94% and 95% and will compile all shaders (this can take from 30min to 2 hours depending on your system).

-For the simulator to store frames or logs you must set the directory in the SimSettings.ini file found in the "Config" folder**

-In order to replay logs you must set the log directories in the Sim Settings.ini file found in the "Config" folder. 

**If you don't have an SSD, setup a RamDisk for storing frames to reduce latency. 

--Note we currently are implementing a UDP network to allow direct memory transfer into Matlab, QT, or ROS connected computer.

## Compiling

-In order to run the project you will need to compile the project and plugins. This can be done either by loading the uproject or after generating MVC files and compiling.

## Accessing Default Level from Editor

-When you first load the default project in UE4 you will see in the editor window a black screen. We have created a blank level that has everything integrated for you that needs to be loaded. All the actors required to run the various elements of the simulator have been dropped in for you to build off of.

-To access the default "blank" level go to the Simulator folder in the Content Browser and double click on Sim_Default. 

-Once loaded hit play to try out the level.

## Flight & Vehicle Controls

-Please use Project Settings->Input to examine all inputs. The most important are described below:

-To move the Multirotor use keyboard controls up/down (forward and back/Pitch), left/right (Roll), n/m altitude, q,e rotation (Yaw).
  
-To move the Car use keyboard controls i/k (gas/reverse), j/l (steer). Vive controls: Right Controller keypad X (steer), Right Controller trigger (gas).

-To move Player View use mouse and w/s (when not in VR mode).

-To lock player to car/UAV toggle C

-To switch between car/UAV press F (Only works when C toggled off)

-To change View offset of following camera press V. You can modify following offset within Level Blueprint in View by Multirotor Section. (Only works when C toggled off)

-To Toggle the HUD with Cam view press H.

## HUD

-The HUD allows you to control all the main Simulator settings from within the game removing the need to hard code anything. Changes to settings are found in Config/SimSettings.ini. If you Switch to Sim Settings inside the HUD you can modify any Sim Setting and Click save.

## Arming/Disarming Flying

-The simulator starts the copter as Armed, but it will not hover until the altitude setpoint is increased. Press N to increase altitude

-If you land the copter it will switch to disarmed. To re-arm the copter press Enter

## Waypoint Flight Missions

-The copter will fly to set waypoints in the Level Blueprint. To start waypoint mission press SpaceBar

## Simulator Controls

All Controls are handled through keyboad
- Toggle UAV Tracking: T
- Toggle GT tracker or Matlab Tracker: Y
- Start Car Simulation: R
- Run Logged Simulations: B
- Pause, unpause Simulations: P
- Switch View when running Simulations: F
- Toggle Camera control in Simulations: C

## Running Simulator with Matlab

-Get Matlab project from website
-Run set tracker
-Matlab will loop printing "waiting" until simulator outputs first init file.
-Start UE4 Simulator
-Press T (Starts GT Tracker, to center Copter over car)
-Press Y (Writes Initial Bounding Box and will wait for Matlab to output gt.txt file for updated bounding box)
-Press R to run Vehicle or optionally use car controls to drive car manually.


## Creating a New Level or Completly New Project

1. Create a new level (This is important since any changes made on Sim_default will conflict with Github) or New project from Unreal (This can be an already created UE4 world). Skip to 7 if creating a new level.
2. Copy the plugins folder to the root folder of your new project
3. Open the default project of the simulator "UAVSimUE4" and migrate the Simulator Folder
4. Open your new project, you should see the migrated content
5. Copy from UAVSimUE4 Config folder: DefaultInput.ini and SimSettings. Paste into the Config folder of you project.
6. Open Project Settings->Input. Import DefaultInput.ini.
7. Load Your New Project or New Level (If compiled with the same engine, you will not need to recompile). 
8. In order to use all the classes and receive input in your new project you need to drop into your level several generic actors (see sim_default as example):

-Simulator/Camera/CamLogger

-Simulator/DefaultLevel/LevelInputActor

-Simulator/DefaultLevel/SimulationActor

-Simulator/Multirotor/Blueprints/MultiRotorMBZIRC

-Simulator/Multirotor/Blueprints/VisionControlBasic

-Simulator/Rover/Blueprints/Rover

-Simulator/Rover/Blueprints/AiBuggyVH

9. Click on Each of these Actors and assign the multirotor or Rover to the actor (Use the dripper button to select).
10. For each actor under Input set Auto Receive Input to Player0. 
11. Set Gamemode. Blueprints->WorldOveride->Edit Vive Game Mode
12. Play in Editor