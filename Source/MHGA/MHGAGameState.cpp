// Fill out your copyright notice in the Description page of Project Settings.


#include "MHGAGameState.h"

#include "Counter/CounterPOS.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AMHGAGameState::AMHGAGameState()
{
	bReplicates = true;
}

void AMHGAGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
		Counter = Cast<ACounterPOS>(UGameplayStatics::GetActorOfClass(GetWorld(), ACounterPOS::StaticClass()));
}

void AMHGAGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMHGAGameState, Counter);
}