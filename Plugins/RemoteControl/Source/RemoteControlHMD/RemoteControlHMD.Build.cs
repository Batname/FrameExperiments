namespace UnrealBuildTool.Rules
{
	public class RemoteControlHMD : ModuleRules
	{
		public RemoteControlHMD(ReadOnlyTargetRules Target ) : base(Target)
		{
            bEnforceIWYU = false;

            PrivateIncludePaths.AddRange(
                new string[] {
					"RemoteControl/Private"
				}
                );

            PublicDependencyModuleNames.AddRange(
				new string[] 
				{
					"Core",
                    "RemoteControl"
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[] 
				{
					"CoreUObject",
					"Engine",
					"Slate",
					"SlateCore",
					"RenderCore",
					"ShaderCore",
                    "HeadMountedDisplay",
                    "InputCore",
					"RHI",
                    "Networking",
                    "ImageWrapper",
                    "SlateRHIRenderer",
                    "Settings",
                    "Renderer"
				}
			);
		}
	}
}
