// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Private/RemoteControlPrivatePCH.h"
#include "Public/RemoteControlBlueprintLibrary.h"
PRAGMA_DISABLE_OPTIMIZATION
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeRemoteControlBlueprintLibrary() {}
// Cross Module References
	REMOTECONTROL_API UFunction* Z_Construct_UFunction_URemoteControlBlueprintLibrary_StartRemoteControl();
	REMOTECONTROL_API UClass* Z_Construct_UClass_URemoteControlBlueprintLibrary();
	REMOTECONTROL_API UFunction* Z_Construct_UFunction_URemoteControlBlueprintLibrary_StopRemoteControl();
	REMOTECONTROL_API UClass* Z_Construct_UClass_URemoteControlBlueprintLibrary_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UBlueprintFunctionLibrary();
	UPackage* Z_Construct_UPackage__Script_RemoteControl();
// End Cross Module References
	void URemoteControlBlueprintLibrary::StaticRegisterNativesURemoteControlBlueprintLibrary()
	{
		UClass* Class = URemoteControlBlueprintLibrary::StaticClass();
		static const TNameNativePtrPair<ANSICHAR> AnsiFuncs[] = {
			{ "StartRemoteControl", (Native)&URemoteControlBlueprintLibrary::execStartRemoteControl },
			{ "StopRemoteControl", (Native)&URemoteControlBlueprintLibrary::execStopRemoteControl },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, AnsiFuncs, ARRAY_COUNT(AnsiFuncs));
	}
	UFunction* Z_Construct_UFunction_URemoteControlBlueprintLibrary_StartRemoteControl()
	{
		struct RemoteControlBlueprintLibrary_eventStartRemoteControl_Parms
		{
			FString Ip;
		};
		UObject* Outer = Z_Construct_UClass_URemoteControlBlueprintLibrary();
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			ReturnFunction = new(EC_InternalUseOnlyConstructor, Outer, TEXT("StartRemoteControl"), RF_Public|RF_Transient|RF_MarkAsNative) UFunction(FObjectInitializer(), nullptr, (EFunctionFlags)0x04022401, 65535, sizeof(RemoteControlBlueprintLibrary_eventStartRemoteControl_Parms));
			UProperty* NewProp_Ip = new(EC_InternalUseOnlyConstructor, ReturnFunction, TEXT("Ip"), RF_Public|RF_Transient|RF_MarkAsNative) UStrProperty(CPP_PROPERTY_BASE(Ip, RemoteControlBlueprintLibrary_eventStartRemoteControl_Parms), 0x0010000000000080);
			ReturnFunction->Bind();
			ReturnFunction->StaticLink();
#if WITH_METADATA
			UMetaData* MetaData = ReturnFunction->GetOutermost()->GetMetaData();
			MetaData->SetValue(ReturnFunction, TEXT("Category"), TEXT("Plugin - Remote Control"));
			MetaData->SetValue(ReturnFunction, TEXT("ModuleRelativePath"), TEXT("Public/RemoteControlBlueprintLibrary.h"));
#endif
		}
		return ReturnFunction;
	}
	UFunction* Z_Construct_UFunction_URemoteControlBlueprintLibrary_StopRemoteControl()
	{
		UObject* Outer = Z_Construct_UClass_URemoteControlBlueprintLibrary();
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			ReturnFunction = new(EC_InternalUseOnlyConstructor, Outer, TEXT("StopRemoteControl"), RF_Public|RF_Transient|RF_MarkAsNative) UFunction(FObjectInitializer(), nullptr, (EFunctionFlags)0x04022401, 65535);
			ReturnFunction->Bind();
			ReturnFunction->StaticLink();
#if WITH_METADATA
			UMetaData* MetaData = ReturnFunction->GetOutermost()->GetMetaData();
			MetaData->SetValue(ReturnFunction, TEXT("Category"), TEXT("Plugin - Remote Control"));
			MetaData->SetValue(ReturnFunction, TEXT("ModuleRelativePath"), TEXT("Public/RemoteControlBlueprintLibrary.h"));
#endif
		}
		return ReturnFunction;
	}
	UClass* Z_Construct_UClass_URemoteControlBlueprintLibrary_NoRegister()
	{
		return URemoteControlBlueprintLibrary::StaticClass();
	}
	UClass* Z_Construct_UClass_URemoteControlBlueprintLibrary()
	{
		static UClass* OuterClass = NULL;
		if (!OuterClass)
		{
			Z_Construct_UClass_UBlueprintFunctionLibrary();
			Z_Construct_UPackage__Script_RemoteControl();
			OuterClass = URemoteControlBlueprintLibrary::StaticClass();
			if (!(OuterClass->ClassFlags & CLASS_Constructed))
			{
				UObjectForceRegistration(OuterClass);
				OuterClass->ClassFlags |= (EClassFlags)0x20100080u;

				OuterClass->LinkChild(Z_Construct_UFunction_URemoteControlBlueprintLibrary_StartRemoteControl());
				OuterClass->LinkChild(Z_Construct_UFunction_URemoteControlBlueprintLibrary_StopRemoteControl());

				OuterClass->AddFunctionToFunctionMapWithOverriddenName(Z_Construct_UFunction_URemoteControlBlueprintLibrary_StartRemoteControl(), "StartRemoteControl"); // 455512628
				OuterClass->AddFunctionToFunctionMapWithOverriddenName(Z_Construct_UFunction_URemoteControlBlueprintLibrary_StopRemoteControl(), "StopRemoteControl"); // 3015207494
				static TCppClassTypeInfo<TCppClassTypeTraits<URemoteControlBlueprintLibrary> > StaticCppClassTypeInfo;
				OuterClass->SetCppTypeInfo(&StaticCppClassTypeInfo);
				OuterClass->StaticLink();
#if WITH_METADATA
				UMetaData* MetaData = OuterClass->GetOutermost()->GetMetaData();
				MetaData->SetValue(OuterClass, TEXT("IncludePath"), TEXT("RemoteControlBlueprintLibrary.h"));
				MetaData->SetValue(OuterClass, TEXT("ModuleRelativePath"), TEXT("Public/RemoteControlBlueprintLibrary.h"));
				MetaData->SetValue(OuterClass, TEXT("ObjectInitializerConstructorDeclared"), TEXT(""));
#endif
			}
		}
		check(OuterClass->GetClass());
		return OuterClass;
	}
	IMPLEMENT_CLASS(URemoteControlBlueprintLibrary, 1828235791);
	static FCompiledInDefer Z_CompiledInDefer_UClass_URemoteControlBlueprintLibrary(Z_Construct_UClass_URemoteControlBlueprintLibrary, &URemoteControlBlueprintLibrary::StaticClass, TEXT("/Script/RemoteControl"), TEXT("URemoteControlBlueprintLibrary"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(URemoteControlBlueprintLibrary);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
PRAGMA_ENABLE_OPTIMIZATION
