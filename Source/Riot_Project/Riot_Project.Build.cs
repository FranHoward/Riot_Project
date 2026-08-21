// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Riot_Project : ModuleRules
{
	public Riot_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Riot_Project",
			"Riot_Project/Variant_Platforming",
			"Riot_Project/Variant_Platforming/Animation",
			"Riot_Project/Variant_Combat",
			"Riot_Project/Variant_Combat/AI",
			"Riot_Project/Variant_Combat/Animation",
			"Riot_Project/Variant_Combat/Gameplay",
			"Riot_Project/Variant_Combat/Interfaces",
			"Riot_Project/Variant_Combat/UI",
			"Riot_Project/Variant_SideScrolling",
			"Riot_Project/Variant_SideScrolling/AI",
			"Riot_Project/Variant_SideScrolling/Gameplay",
			"Riot_Project/Variant_SideScrolling/Interfaces",
			"Riot_Project/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
