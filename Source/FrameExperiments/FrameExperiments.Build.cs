// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FrameExperiments : ModuleRules
{
	public FrameExperiments(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore",
                    "Slate",
                    "SlateCore",
                    "RenderCore",
                    "ShaderCore",
                    "RHI",
                    "Networking",
                    "ImageWrapper",
                    "SlateRHIRenderer",
                    "Sockets",
                    "Settings"
        });
	}
}
