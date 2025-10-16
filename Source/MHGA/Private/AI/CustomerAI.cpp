// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomerAI.h"

#include "AIController.h"
#include "AI/CustomerFSM.h"
#include "DSP/AudioDebuggingUtilities.h"

// Sets default values
ACustomerAI::ACustomerAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	fsm = CreateDefaultSubobject<UCustomerFSM>(TEXT("FSM"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

// Called when the game starts or when spawned
void ACustomerAI::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACustomerAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

