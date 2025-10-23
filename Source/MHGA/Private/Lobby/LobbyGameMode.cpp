// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "Lobby/LobbyBoard.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// PlayerArray에 이미 새로 들어온 플레이어 포함되어 있음
	AGameStateBase* gs = GetGameState<AGameStateBase>();
	if (LobbyBoard)
		LobbyBoard->MulticastRPC_Refresh(gs->PlayerArray.Num());
}
