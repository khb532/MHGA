// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

class ALobbyBoard;
/**
 * 
 */
UCLASS()
class MHGA_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	UPROPERTY(EditAnywhere)
	ALobbyBoard* LobbyBoard;

public:
	void SetLobbyBoard(ALobbyBoard* InLobbyBoard){LobbyBoard = InLobbyBoard;}
};
