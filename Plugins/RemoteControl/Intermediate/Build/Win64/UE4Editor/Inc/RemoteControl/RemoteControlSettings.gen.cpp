// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Private/RemoteControlPrivatePCH.h"
#include "Private/RemoteControlSettings.h"
PRAGMA_DISABLE_OPTIMIZATION
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeRemoteControlSettings() {}
// Cross Module References
	REMOTECONTROL_API UEnum* Z_Construct_UEnum_RemoteControl_ERemoteControlCommunication();
	UPackage* Z_Construct_UPackage__Script_RemoteControl();
	REMOTECONTROL_API UEnum* Z_Construct_UEnum_RemoteControl_ERemoteResolution();
	REMOTECONTROL_API UClass* Z_Construct_UClass_URemoteControlSettings_NoRegister();
	REMOTECONTROL_API UClass* Z_Construct_UClass_URemoteControlSettings();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
// End Cross Module References
static UEnum* ERemoteControlCommunication_StaticEnum()
{
	static UEnum* Singleton = nullptr;
	if (!Singleton)
	{
		Singleton = GetStaticEnum(Z_Construct_UEnum_RemoteControl_ERemoteControlCommunication, Z_Construct_UPackage__Script_RemoteControl(), TEXT("ERemoteControlCommunication"));
	}
	return Singleton;
}
static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_ERemoteControlCommunication(ERemoteControlCommunication_StaticEnum, TEXT("/Script/RemoteControl"), TEXT("ERemoteControlCommunication"), false, nullptr, nullptr);
	UEnum* Z_Construct_UEnum_RemoteControl_ERemoteControlCommunication()
	{
		UPackage* Outer = Z_Construct_UPackage__Script_RemoteControl();
		extern uint32 Get_Z_Construct_UEnum_RemoteControl_ERemoteControlCommunication_CRC();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("ERemoteControlCommunication"), 0, Get_Z_Construct_UEnum_RemoteControl_ERemoteControlCommunication_CRC(), false);
		if (!ReturnEnum)
		{
			ReturnEnum = new(EC_InternalUseOnlyConstructor, Outer, TEXT("ERemoteControlCommunication"), RF_Public|RF_Transient|RF_MarkAsNative) UEnum(FObjectInitializer());
			TArray<TPair<FName, int64>> EnumNames;
			EnumNames.Emplace(TEXT("ERemoteControlCommunication::USB"), 0);
			EnumNames.Emplace(TEXT("ERemoteControlCommunication::WiFi"), 1);
			EnumNames.Emplace(TEXT("ERemoteControlCommunication::ERemoteControlCommunication_MAX"), 2);
			ReturnEnum->SetEnums(EnumNames, UEnum::ECppForm::EnumClass);
			ReturnEnum->CppType = TEXT("ERemoteControlCommunication");
#if WITH_METADATA
			UMetaData* MetaData = ReturnEnum->GetOutermost()->GetMetaData();
			MetaData->SetValue(ReturnEnum, TEXT("BlueprintType"), TEXT("true"));
			MetaData->SetValue(ReturnEnum, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
			MetaData->SetValue(ReturnEnum, TEXT("WiFi.DisplayName"), TEXT("Wi-Fi"));
#endif
		}
		return ReturnEnum;
	}
	uint32 Get_Z_Construct_UEnum_RemoteControl_ERemoteControlCommunication_CRC() { return 45298412U; }
static UEnum* ERemoteResolution_StaticEnum()
{
	static UEnum* Singleton = nullptr;
	if (!Singleton)
	{
		Singleton = GetStaticEnum(Z_Construct_UEnum_RemoteControl_ERemoteResolution, Z_Construct_UPackage__Script_RemoteControl(), TEXT("ERemoteResolution"));
	}
	return Singleton;
}
static FCompiledInDeferEnum Z_CompiledInDeferEnum_UEnum_ERemoteResolution(ERemoteResolution_StaticEnum, TEXT("/Script/RemoteControl"), TEXT("ERemoteResolution"), false, nullptr, nullptr);
	UEnum* Z_Construct_UEnum_RemoteControl_ERemoteResolution()
	{
		UPackage* Outer = Z_Construct_UPackage__Script_RemoteControl();
		extern uint32 Get_Z_Construct_UEnum_RemoteControl_ERemoteResolution_CRC();
		static UEnum* ReturnEnum = FindExistingEnumIfHotReloadOrDynamic(Outer, TEXT("ERemoteResolution"), 0, Get_Z_Construct_UEnum_RemoteControl_ERemoteResolution_CRC(), false);
		if (!ReturnEnum)
		{
			ReturnEnum = new(EC_InternalUseOnlyConstructor, Outer, TEXT("ERemoteResolution"), RF_Public|RF_Transient|RF_MarkAsNative) UEnum(FObjectInitializer());
			TArray<TPair<FName, int64>> EnumNames;
			EnumNames.Emplace(TEXT("ERemoteResolution::r128"), 0);
			EnumNames.Emplace(TEXT("ERemoteResolution::r256"), 1);
			EnumNames.Emplace(TEXT("ERemoteResolution::r512"), 2);
			EnumNames.Emplace(TEXT("ERemoteResolution::r768"), 3);
			EnumNames.Emplace(TEXT("ERemoteResolution::r1024"), 4);
			EnumNames.Emplace(TEXT("ERemoteResolution::ERemoteResolution_MAX"), 5);
			ReturnEnum->SetEnums(EnumNames, UEnum::ECppForm::EnumClass);
			ReturnEnum->CppType = TEXT("ERemoteResolution");
#if WITH_METADATA
			UMetaData* MetaData = ReturnEnum->GetOutermost()->GetMetaData();
			MetaData->SetValue(ReturnEnum, TEXT("BlueprintType"), TEXT("true"));
			MetaData->SetValue(ReturnEnum, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
			MetaData->SetValue(ReturnEnum, TEXT("r1024.DisplayName"), TEXT("1024"));
			MetaData->SetValue(ReturnEnum, TEXT("r128.DisplayName"), TEXT("128"));
			MetaData->SetValue(ReturnEnum, TEXT("r256.DisplayName"), TEXT("256"));
			MetaData->SetValue(ReturnEnum, TEXT("r512.DisplayName"), TEXT("512"));
			MetaData->SetValue(ReturnEnum, TEXT("r768.DisplayName"), TEXT("768"));
#endif
		}
		return ReturnEnum;
	}
	uint32 Get_Z_Construct_UEnum_RemoteControl_ERemoteResolution_CRC() { return 2375656533U; }
	void URemoteControlSettings::StaticRegisterNativesURemoteControlSettings()
	{
	}
	UClass* Z_Construct_UClass_URemoteControlSettings_NoRegister()
	{
		return URemoteControlSettings::StaticClass();
	}
	UClass* Z_Construct_UClass_URemoteControlSettings()
	{
		static UClass* OuterClass = NULL;
		if (!OuterClass)
		{
			Z_Construct_UClass_UObject();
			Z_Construct_UPackage__Script_RemoteControl();
			OuterClass = URemoteControlSettings::StaticClass();
			if (!(OuterClass->ClassFlags & CLASS_Constructed))
			{
				UObjectForceRegistration(OuterClass);
				OuterClass->ClassFlags |= (EClassFlags)0x20100084u;


				CPP_BOOL_PROPERTY_BITMASK_STRUCT(bEnableHeadTracking, URemoteControlSettings);
				UProperty* NewProp_bEnableHeadTracking = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("bEnableHeadTracking"), RF_Public|RF_Transient|RF_MarkAsNative) UBoolProperty(FObjectInitializer(), EC_CppProperty, CPP_BOOL_PROPERTY_OFFSET(bEnableHeadTracking, URemoteControlSettings), 0x0010000000004001, CPP_BOOL_PROPERTY_BITMASK(bEnableHeadTracking, URemoteControlSettings), sizeof(bool), true);
				UProperty* NewProp_AdditionalControllers = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("AdditionalControllers"), RF_Public|RF_Transient|RF_MarkAsNative) UArrayProperty(CPP_PROPERTY_BASE(AdditionalControllers, URemoteControlSettings), 0x0010000000004001);
				UProperty* NewProp_AdditionalControllers_Inner = new(EC_InternalUseOnlyConstructor, NewProp_AdditionalControllers, TEXT("AdditionalControllers"), RF_Public|RF_Transient|RF_MarkAsNative) UStrProperty(FObjectInitializer(), EC_CppProperty, 0, 0x0000000000004000);
				CPP_BOOL_PROPERTY_BITMASK_STRUCT(bEnableAdditionalControllers, URemoteControlSettings);
				UProperty* NewProp_bEnableAdditionalControllers = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("bEnableAdditionalControllers"), RF_Public|RF_Transient|RF_MarkAsNative) UBoolProperty(FObjectInitializer(), EC_CppProperty, CPP_BOOL_PROPERTY_OFFSET(bEnableAdditionalControllers, URemoteControlSettings), 0x0010000000004001, CPP_BOOL_PROPERTY_BITMASK(bEnableAdditionalControllers, URemoteControlSettings), sizeof(bool), true);
				UProperty* NewProp_IpAddress = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("IpAddress"), RF_Public|RF_Transient|RF_MarkAsNative) UStrProperty(CPP_PROPERTY_BASE(IpAddress, URemoteControlSettings), 0x0010000000004001);
				UProperty* NewProp_Communication = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("Communication"), RF_Public|RF_Transient|RF_MarkAsNative) UEnumProperty(CPP_PROPERTY_BASE(Communication, URemoteControlSettings), 0x0010000000004001, Z_Construct_UEnum_RemoteControl_ERemoteControlCommunication());
				UProperty* NewProp_Communication_Underlying = new(EC_InternalUseOnlyConstructor, NewProp_Communication, TEXT("UnderlyingType"), RF_Public|RF_Transient|RF_MarkAsNative) UByteProperty(FObjectInitializer(), EC_CppProperty, 0, 0x0000000000000000);
				UProperty* NewProp_Port = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("Port"), RF_Public|RF_Transient|RF_MarkAsNative) UIntProperty(CPP_PROPERTY_BASE(Port, URemoteControlSettings), 0x0010000000004014);
				UProperty* NewProp_Quality = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("Quality"), RF_Public|RF_Transient|RF_MarkAsNative) UIntProperty(CPP_PROPERTY_BASE(Quality, URemoteControlSettings), 0x0010000000004001);
				UProperty* NewProp_Resolution = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("Resolution"), RF_Public|RF_Transient|RF_MarkAsNative) UEnumProperty(CPP_PROPERTY_BASE(Resolution, URemoteControlSettings), 0x0010000000004001, Z_Construct_UEnum_RemoteControl_ERemoteResolution());
				UProperty* NewProp_Resolution_Underlying = new(EC_InternalUseOnlyConstructor, NewProp_Resolution, TEXT("UnderlyingType"), RF_Public|RF_Transient|RF_MarkAsNative) UByteProperty(FObjectInitializer(), EC_CppProperty, 0, 0x0000000000000000);
				UProperty* NewProp_FrameRate = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("FrameRate"), RF_Public|RF_Transient|RF_MarkAsNative) UIntProperty(CPP_PROPERTY_BASE(FrameRate, URemoteControlSettings), 0x0010000000004001);
				CPP_BOOL_PROPERTY_BITMASK_STRUCT(bIsEnabled, URemoteControlSettings);
				UProperty* NewProp_bIsEnabled = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("bIsEnabled"), RF_Public|RF_Transient|RF_MarkAsNative) UBoolProperty(FObjectInitializer(), EC_CppProperty, CPP_BOOL_PROPERTY_OFFSET(bIsEnabled, URemoteControlSettings), 0x0010000000004001, CPP_BOOL_PROPERTY_BITMASK(bIsEnabled, URemoteControlSettings), sizeof(bool), true);
				OuterClass->ClassConfigName = FName(TEXT("EditorSettings"));
				static TCppClassTypeInfo<TCppClassTypeTraits<URemoteControlSettings> > StaticCppClassTypeInfo;
				OuterClass->SetCppTypeInfo(&StaticCppClassTypeInfo);
				OuterClass->StaticLink();
#if WITH_METADATA
				UMetaData* MetaData = OuterClass->GetOutermost()->GetMetaData();
				MetaData->SetValue(OuterClass, TEXT("IncludePath"), TEXT("RemoteControlSettings.h"));
				MetaData->SetValue(OuterClass, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
				MetaData->SetValue(OuterClass, TEXT("ObjectInitializerConstructorDeclared"), TEXT(""));
				MetaData->SetValue(NewProp_bEnableHeadTracking, TEXT("Category"), TEXT("VR"));
				MetaData->SetValue(NewProp_bEnableHeadTracking, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
				MetaData->SetValue(NewProp_bEnableHeadTracking, TEXT("ToolTip"), TEXT("Only affects editor viewport (Not affects Launch VR Preview. Always true in VR Preview)"));
				MetaData->SetValue(NewProp_AdditionalControllers, TEXT("Category"), TEXT("AdditionalControllers"));
				MetaData->SetValue(NewProp_AdditionalControllers, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
				MetaData->SetValue(NewProp_AdditionalControllers, TEXT("ToolTip"), TEXT("Please enter ip address of each device for additional controller. Additional controllers doesn't support USB communication."));
				MetaData->SetValue(NewProp_bEnableAdditionalControllers, TEXT("Category"), TEXT("AdditionalControllers"));
				MetaData->SetValue(NewProp_bEnableAdditionalControllers, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
				MetaData->SetValue(NewProp_bEnableAdditionalControllers, TEXT("ToolTip"), TEXT("If you want to enable additional controllers, check it."));
				MetaData->SetValue(NewProp_IpAddress, TEXT("Category"), TEXT("FirstController"));
				MetaData->SetValue(NewProp_IpAddress, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
				MetaData->SetValue(NewProp_IpAddress, TEXT("ToolTip"), TEXT("If you want to connect your mobile device via Wi-Fi, you need put ip address of the device."));
				MetaData->SetValue(NewProp_Communication, TEXT("Category"), TEXT("FirstController"));
				MetaData->SetValue(NewProp_Communication, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
				MetaData->SetValue(NewProp_Communication, TEXT("ToolTip"), TEXT("!!! IMPORTANT !!! iOS devices doesn't support USB communication. Will be support in the future releases.\n\nCommunication mode between your mobile device and editor."));
				MetaData->SetValue(NewProp_Port, TEXT("Category"), TEXT("GeneralSettings"));
				MetaData->SetValue(NewProp_Port, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
				MetaData->SetValue(NewProp_Port, TEXT("ToolTip"), TEXT("@todo: Port is fixed and cannot be changed currently."));
				MetaData->SetValue(NewProp_Quality, TEXT("Category"), TEXT("GeneralSettings"));
				MetaData->SetValue(NewProp_Quality, TEXT("ClampMin"), TEXT("1"));
				MetaData->SetValue(NewProp_Quality, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
				MetaData->SetValue(NewProp_Quality, TEXT("ToolTip"), TEXT("Quality of compressed stream.\nLow: Fastest but lower quality\nHigh: Slowest but highest quality.\n\nDo not change this setting too high unless you are sure your mobile device can handle it. Recommended: 60"));
				MetaData->SetValue(NewProp_Quality, TEXT("UIMax"), TEXT("100"));
				MetaData->SetValue(NewProp_Quality, TEXT("UIMin"), TEXT("5"));
				MetaData->SetValue(NewProp_Resolution, TEXT("Category"), TEXT("GeneralSettings"));
				MetaData->SetValue(NewProp_Resolution, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
				MetaData->SetValue(NewProp_Resolution, TEXT("ToolTip"), TEXT("If you change this setting, you need to re-start stream.\nIf you prefer high resolution make sure your mobile device can handle it."));
				MetaData->SetValue(NewProp_FrameRate, TEXT("Category"), TEXT("GeneralSettings"));
				MetaData->SetValue(NewProp_FrameRate, TEXT("ClampMin"), TEXT("1"));
				MetaData->SetValue(NewProp_FrameRate, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
				MetaData->SetValue(NewProp_FrameRate, TEXT("ToolTip"), TEXT("Frame rate of stream. Do not change this to high unless you are sure your mobile device can handle it. Recommended: 25, 30"));
				MetaData->SetValue(NewProp_FrameRate, TEXT("UIMax"), TEXT("60"));
				MetaData->SetValue(NewProp_FrameRate, TEXT("UIMin"), TEXT("5"));
				MetaData->SetValue(NewProp_bIsEnabled, TEXT("Category"), TEXT("GeneralSettings"));
				MetaData->SetValue(NewProp_bIsEnabled, TEXT("ModuleRelativePath"), TEXT("Private/RemoteControlSettings.h"));
				MetaData->SetValue(NewProp_bIsEnabled, TEXT("ToolTip"), TEXT("Enable remote control."));
#endif
			}
		}
		check(OuterClass->GetClass());
		return OuterClass;
	}
	IMPLEMENT_CLASS(URemoteControlSettings, 4033016347);
	static FCompiledInDefer Z_CompiledInDefer_UClass_URemoteControlSettings(Z_Construct_UClass_URemoteControlSettings, &URemoteControlSettings::StaticClass, TEXT("/Script/RemoteControl"), TEXT("URemoteControlSettings"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(URemoteControlSettings);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
PRAGMA_ENABLE_OPTIMIZATION
