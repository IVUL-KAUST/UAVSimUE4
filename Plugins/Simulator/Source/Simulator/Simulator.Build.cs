// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class Simulator : ModuleRules
	{
		public Simulator(TargetInfo Target)
		{
			PublicIncludePaths.AddRange(
				new string[] {
					// ... add public include paths required here ...
                    "Simulator/Public"
                }
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"Simulator/Private"
                    
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
                "CoreUObject",
                "Engine",
                "InputCore",

                "RHI",
                "RenderCore",

                "HTTP",
                "UMG", "Slate", "SlateCore",

                "ImageWrapper",

                "PhysX", "APEX",

                "Networking", "Sockets",

                "AIModule"
            
                }
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);
		}
	}
}