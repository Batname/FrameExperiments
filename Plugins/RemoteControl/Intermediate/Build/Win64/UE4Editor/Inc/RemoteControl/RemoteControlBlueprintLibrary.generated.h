// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "ObjectMacros.h"
#include "ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef REMOTECONTROL_RemoteControlBlueprintLibrary_generated_h
#error "RemoteControlBlueprintLibrary.generated.h already included, missing '#pragma once' in RemoteControlBlueprintLibrary.h"
#endif
#define REMOTECONTROL_RemoteControlBlueprintLibrary_generated_h

#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execStopRemoteControl) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		URemoteControlBlueprintLibrary::StopRemoteControl(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execStartRemoteControl) \
	{ \
		P_GET_PROPERTY(UStrProperty,Z_Param_Ip); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		URemoteControlBlueprintLibrary::StartRemoteControl(Z_Param_Ip); \
		P_NATIVE_END; \
	}


#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execStopRemoteControl) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		URemoteControlBlueprintLibrary::StopRemoteControl(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execStartRemoteControl) \
	{ \
		P_GET_PROPERTY(UStrProperty,Z_Param_Ip); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		URemoteControlBlueprintLibrary::StartRemoteControl(Z_Param_Ip); \
		P_NATIVE_END; \
	}


#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesURemoteControlBlueprintLibrary(); \
	friend REMOTECONTROL_API class UClass* Z_Construct_UClass_URemoteControlBlueprintLibrary(); \
public: \
	DECLARE_CLASS(URemoteControlBlueprintLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/RemoteControl"), NO_API) \
	DECLARE_SERIALIZER(URemoteControlBlueprintLibrary) \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_INCLASS \
private: \
	static void StaticRegisterNativesURemoteControlBlueprintLibrary(); \
	friend REMOTECONTROL_API class UClass* Z_Construct_UClass_URemoteControlBlueprintLibrary(); \
public: \
	DECLARE_CLASS(URemoteControlBlueprintLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/RemoteControl"), NO_API) \
	DECLARE_SERIALIZER(URemoteControlBlueprintLibrary) \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API URemoteControlBlueprintLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(URemoteControlBlueprintLibrary) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, URemoteControlBlueprintLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(URemoteControlBlueprintLibrary); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API URemoteControlBlueprintLibrary(URemoteControlBlueprintLibrary&&); \
	NO_API URemoteControlBlueprintLibrary(const URemoteControlBlueprintLibrary&); \
public:


#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API URemoteControlBlueprintLibrary(URemoteControlBlueprintLibrary&&); \
	NO_API URemoteControlBlueprintLibrary(const URemoteControlBlueprintLibrary&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, URemoteControlBlueprintLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(URemoteControlBlueprintLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(URemoteControlBlueprintLibrary)


#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_PRIVATE_PROPERTY_OFFSET
#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_6_PROLOG
#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_PRIVATE_PROPERTY_OFFSET \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_RPC_WRAPPERS \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_INCLASS \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_PRIVATE_PROPERTY_OFFSET \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_RPC_WRAPPERS_NO_PURE_DECLS \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_INCLASS_NO_PURE_DECLS \
	FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h_10_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FrameExperiments_Plugins_RemoteControl_Source_RemoteControl_Public_RemoteControlBlueprintLibrary_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
