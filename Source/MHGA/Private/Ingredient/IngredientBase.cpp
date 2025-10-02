// Fill out your copyright notice in the Description page of Project Settings.


#include "Ingredient/IngredientBase.h"


AIngredientBase::AIngredientBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void AIngredientBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AIngredientBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIngredientBase::OnGrabbed()
{
}

