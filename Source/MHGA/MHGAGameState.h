// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MHGAGameState.generated.h"

class ACounterPOS;

UCLASS()
class MHGA_API AMHGAGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMHGAGameState();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	ACounterPOS* Counter;

public:
	ACounterPOS* GetCounter() {return Counter;}
};
