// Copyright Epic Games, Inc. All Rights Reserved.

#include "PompastronautesGameMode.h"
#include "PompastronautesCharacter.h"
#include "UObject/ConstructorHelpers.h"

APompastronautesGameMode::APompastronautesGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
