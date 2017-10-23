// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FrameExperimentsGameMode.h"
#include "FrameExperimentsPawn.h"

AFrameExperimentsGameMode::AFrameExperimentsGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AFrameExperimentsPawn::StaticClass();
}
