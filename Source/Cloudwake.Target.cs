using UnrealBuildTool;
using System.Collections.Generic;
public class CloudwakeTarget : TargetRules {
 public CloudwakeTarget(TargetInfo Target) : base(Target) {
  Type = TargetType.Game; DefaultBuildSettings = BuildSettingsVersion.V7;
  ExtraModuleNames.Add("Cloudwake");
 }
}
