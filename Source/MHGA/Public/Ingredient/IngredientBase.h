// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BurgerData.h"
#include "GrabableProps.h"
#include "GameFramework/Actor.h"
#include "IngredientBase.generated.h"

enum class EIngredient : uint8;

UCLASS()
class MHGA_API AIngredientBase : public AActor, public IGrabableProps
{
	GENERATED_BODY()

public:
	AIngredientBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Ingredients")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Ingredients")
	EIngredient IngType = EIngredient::None;
	
public:
	virtual void OnGrabbed() override;
};
