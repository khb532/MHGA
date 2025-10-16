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

/* Method */
public:
	AIngredientBase();

	virtual void OnGrabbed() override;
	virtual void OnPut() override;
	virtual void OnUse() override;

	EIngredient GetIngType() { return IngType; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	

private:

	
/* Field */
public:
		
protected:
	UPROPERTY(EditAnywhere, Category = "Ingredients")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Ingredients")
	EIngredient IngType = EIngredient::None;

private:

	
};
