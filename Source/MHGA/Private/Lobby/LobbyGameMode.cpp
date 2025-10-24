// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyGameMode.h"

#include "MHGA.h"
#include "MHGAGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/LobbyBoard.h"
#include "Lobby/LobbyGameState.h"

FString ALobbyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	const FString& Options, const FString& Portal)
{
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	FString Nick = UGameplayStatics::ParseOption(Options, TEXT("Nick"));
	if (NewPlayerController && NewPlayerController->PlayerState && !Nick.IsEmpty())
	{
		NewPlayerController->PlayerState->SetPlayerName(Nick);
	}
	return Result;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PRINTINFO();

	if (ALobbyGameState* LGS = GetGameState<ALobbyGameState>())
	{
		if (APlayerState* PS = NewPlayer ? NewPlayer->PlayerState : nullptr)
			LGS->Server_AddPlayerName(PS->GetPlayerName());
	}
	else
	{
		PRINTINFO();
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	if (ALobbyGameState* LGS = GetGameState<ALobbyGameState>())
	{
		if (APlayerState* PS = Exiting ? Exiting->PlayerState : nullptr)
			LGS->Server_RemovePlayerName(PS->GetPlayerName());
	}
	else
	{
		PRINTINFO();
	}

	Super::Logout(Exiting);
}