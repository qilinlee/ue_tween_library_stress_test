// Copyright Bob Berkebile and Jared Therriault 2014

using UnrealBuildTool;
using System.IO;

public class iTween : ModuleRules
{
    public iTween(ReadOnlyTargetRules target) : base(target)
    {
        PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Private") });
        PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Public") });

        PrivateDependencyModuleNames.AddRange(new string[] 
            { "Engine", "InputCore", "Core", "Slate", "SlateCore", "CoreUObject", "UMG" });

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
    }
}
