using UnrealBuildTool;
using System.Collections.Generic;
public class CloudwakeEditorTarget : TargetRules {
 public CloudwakeEditorTarget(TargetInfo Target) : base(Target) {
  Type = TargetType.Editor; DefaultBuildSettings = BuildSettingsVersion.V7;
  ExtraModuleNames.Add("Cloudwake");
 }
}
