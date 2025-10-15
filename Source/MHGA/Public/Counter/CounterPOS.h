// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BurgerData.h"
#include "CounterPOS.generated.h"

class UCounterUI;
class UWidgetComponent;

UCLASS()
class MHGA_API ACounterPOS : public AActor
{
	GENERATED_BODY()

public:
	ACounterPOS();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(EditAnywhere)
	UWidgetComponent* WidgetComponent;
	
	UPROPERTY(EditAnywhere)
	UCounterUI* CounterUI;

	//UPROPERTY(EditAnywhere)
	//AActor* CurrentCustomer;
public:
	//TODO : 손님 get, set
};
