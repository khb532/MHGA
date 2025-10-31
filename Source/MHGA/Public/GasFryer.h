// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "GasFryer.generated.h"

UCLASS()
class MHGA_API AGasFryer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGasFryer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* fryer;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* basketL;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* basketR;

	UPROPERTY(EditAnywhere)
	FVector downPosL;
	UPROPERTY(EditAnywhere)
	FVector downPosR;
	UPROPERTY(EditAnywhere)
	float cookTime = 17.f;
	float moveDuration = 3.f;

	FVector upPosL;
	FVector upPosR;
	FTimerHandle cookTimer;

	bool bIsMovingDown = false;
	bool bIsMovingUp = false;
	float movementStartTime = 0;

	void StartCooking();
	void OnCookingFinished();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MovingUp();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MovingDown();
};
