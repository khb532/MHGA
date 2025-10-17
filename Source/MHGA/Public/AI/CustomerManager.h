// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "CustomerManager.generated.h"

UCLASS()
class MHGA_API ACustomerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 손님 스폰 시간
	FTimerHandle spawnTimer;
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float minTime = 1;
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float maxTime = 5;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	ATargetPoint* spawnPoint;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class ACustomerAI> spawnFactory;

	void SpawnCustomer();

	// 손님 줄세우기
	UPROPERTY(EditAnywhere, Category = "Waiting Order")
	TArray<ATargetPoint*> waitingPoints;
	// 줄에서 대기중인 손님열
	UPROPERTY(EditAnywhere, Category = "Waiting Order")
	TArray<ACustomerAI*> waitingCustomers;
	// 줄 밖에서 대기중인 손님열
	UPROPERTY(EditAnywhere, Category = "Waiting Order")
	TArray<ACustomerAI*> wanderingCustomers;
	
	UFUNCTION()
	ATargetPoint* RequestWaitingPoint(ACustomerAI* customer);

	UFUNCTION()
	void OnCustomerFinished(ACustomerAI* customer);

	UFUNCTION()
	void UpdateWaitingPosition();
	UFUNCTION()
	void CallNextCustomerFromWandering();

};
