// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "ObjectMacros.h"
#include "ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef REMOTECONTROL_RemoteControlSettings_generated_h
#error "RemoteControlSettings.generated.h already included, missing '#pragma once' in RemoteControlSettings.h"
#endif
#define REMOTECONTROL_RemoteControlSettings_generated_h

#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_RPC_WRAPPERS
#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_RPC_WRAPPERS_NO_PURE_DECLS
#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesURemoteControlSettings(); \
	friend REMOTECONTROL_API class UClass* Z_Construct_UClass_URemoteControlSettings(); \
public: \
	DECLARE_CLASS(URemoteControlSettings, UObject, COMPILED_IN_FLAGS(0 | CLASS_Config), 0, TEXT("/Script/RemoteControl"), NO_API) \
	DECLARE_SERIALIZER(URemoteControlSettings) \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC}; \
	static const TCHAR* StaticConfigName() {return TEXT("EditorSettings");} \



#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_INCLASS \
private: \
	static void StaticRegisterNativesURemoteControlSettings(); \
	friend REMOTECONTROL_API class UClass* Z_Construct_UClass_URemoteControlSettings(); \
public: \
	DECLARE_CLASS(URemoteControlSettings, UObject, COMPILED_IN_FLAGS(0 | CLASS_Config), 0, TEXT("/Script/RemoteControl"), NO_API) \
	DECLARE_SERIALIZER(URemoteControlSettings) \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC}; \
	static const TCHAR* StaticConfigName() {return TEXT("EditorSettings");} \



#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API URemoteControlSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(URemoteControlSettings) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, URemoteControlSettings); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(URemoteControlSettings); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API URemoteControlSettings(URemoteControlSettings&&); \
	NO_API URemoteControlSettings(const URemoteControlSettings&); \
public:


#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API URemoteControlSettings(URemoteControlSettings&&); \
	NO_API URemoteControlSettings(const URemoteControlSettings&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, URemoteControlSettings); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(URemoteControlSettings); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(URemoteControlSettings)


#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_PRIVATE_PROPERTY_OFFSET
#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_22_PROLOG
#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_PRIVATE_PROPERTY_OFFSET \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_RPC_WRAPPERS \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_INCLASS \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_PRIVATE_PROPERTY_OFFSET \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_RPC_WRAPPERS_NO_PURE_DECLS \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_INCLASS_NO_PURE_DECLS \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h_25_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Private_RemoteControlSettings_h


#define FOREACH_ENUM_EREMOTECONTROLCOMMUNICATION(op) \
	op(ERemoteControlCommunication::USB) \
	op(ERemoteControlCommunication::WiFi) 
#define FOREACH_ENUM_EREMOTERESOLUTION(op) \
	op(ERemoteResolution::r128) \
	op(ERemoteResolution::r256) \
	op(ERemoteResolution::r512) \
	op(ERemoteResolution::r768) \
	op(ERemoteResolution::r1024) 
PRAGMA_ENABLE_DEPRECATION_WARNINGS
