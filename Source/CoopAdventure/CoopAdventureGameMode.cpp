// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoopAdventureGameMode.h"
#include "CoopAdventureCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACoopAdventureGameMode::ACoopAdventureGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
