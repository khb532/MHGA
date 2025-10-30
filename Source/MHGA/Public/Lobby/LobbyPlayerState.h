// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

class UCharacterSelectUI;
/**
 * 
 */
UCLASS()
class MHGA_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ALobbyPlayerState();
	
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCharacterSelectUI> SelectUIClass;

public:
	UFUNCTION(Client, reliable)
	void ClientRPC_MakeReadyUI();
	
	//캐릭터 선택 관련 rpc
	UFUNCTION(Server, Reliable)
	void ServerRPC_SelectPlayer(int32 meshIdx);
};
