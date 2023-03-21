// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GASDemo : ModuleRules
{
	public GASDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay"});
        PublicDependencyModuleNames.AddRange(new string[] { "AIModule" });
        PublicDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks" });
    }
}
