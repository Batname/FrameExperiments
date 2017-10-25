namespace UnrealBuildTool.Rules
{
	public class RemoteControl : ModuleRules
	{
		public RemoteControl(ReadOnlyTargetRules Target ) : base(Target)
		{
            bEnforceIWYU = false;

            //AddThirdPartyPrivateStaticDependencies(Target, "RemoteControl");

            PublicDependencyModuleNames.AddRange(
				new string[] 
				{
					"Core",
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
                    "InputCore",
					"RHI",
                    "Networking",
                    "ImageWrapper",
                    "SlateRHIRenderer",
                    "Sockets",
                    "Settings"
				}
			);
		}
	}
}
