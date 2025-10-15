// Copyright Epic Games, Inc. All Rights Reserved.

#include "MHGAGameMode.h"

#include "MHGAGameState.h"
#include "Player/MHGACharacter.h"
#include "Player/MHGAPlayerController.h"

AMHGAGameMode::AMHGAGameMode()
{
	ConstructorHelpers::FClassFinder<AMHGACharacter> ch(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Player/BP_Player.BP_Player_C'"));
	if (ch.Succeeded())
		DefaultPawnClass = ch.Class;
	PlayerControllerClass = AMHGAPlayerController::StaticClass();
	GameStateClass = AMHGAGameState::StaticClass();
}
