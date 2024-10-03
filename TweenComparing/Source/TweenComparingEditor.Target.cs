// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TweenComparingEditorTarget : TargetRules
{
	public TweenComparingEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
        //DefaultBuildSettings = BuildSettingsVersion.V2;
        BuildEnvironment = TargetBuildEnvironment.Unique;
        ExtraModuleNames.AddRange( new string[] { "TweenComparing" } );
	}
}
