// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

class ALobbyBoard;
class ALobbyPlayerState;
class ALobbyGameState;

/**
 * 
 */
UCLASS()
class MHGA_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ALobbyGameMode();

protected:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<APawn>> characterList;
	
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = L"") override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	virtual void PostSeamlessTravel() override;
	virtual void BeginPlay() override;

	void UpdatePlayerCount();

private:
	void RegisterLobbyPlayer(ALobbyPlayerState* PlayerState);
	void ProcessPendingLobbyPlayers();
	bool ApplyPlayerToLobby(ALobbyGameState* GameState, ALobbyPlayerState* PlayerState);

	UPROPERTY()
	TArray<TWeakObjectPtr<ALobbyPlayerState>> PendingLobbyPlayers;
};
