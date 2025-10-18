// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "CustomerManager.generated.h"

// ADDED: APickupZone 이라는 클래스가 있다고 컴파일러에게 미리 알려줍니다.
class APickupZone; 
class ACustomerAI;
class ATargetPoint;

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

	// -- 손님 줄세우기--
	// 주문 대기열 위치
	UPROPERTY(EditAnywhere, Category = "Waiting Order")
	TArray<ATargetPoint*> waitingPoints;
	// 픽업 위치
	UPROPERTY(EditAnywhere, Category = "Waiting Order")
	TArray<ATargetPoint*> pickupPoints;
	// 줄에서 대기중인 손님열
	UPROPERTY(EditAnywhere, Category = "Waiting Order")
	TArray<ACustomerAI*> waitingCustomers;
	// 줄 밖에서 대기중인 손님열
	UPROPERTY(EditAnywhere, Category = "Waiting Order")
	TArray<ACustomerAI*> wanderingCustomers;
	
	UFUNCTION()
	ATargetPoint* RequestWaitingPoint(ACustomerAI* customer);
	UFUNCTION()
	ATargetPoint* RequestPickupPoint();
	UFUNCTION()
	ATargetPoint* RequestExitPoint();

	UFUNCTION()
	void OnCustomerFinished(ACustomerAI* customer);
	// 줄 밖에서 대기중인 손님열
	UPROPERTY(EditAnywhere, Category = "Waiting Order")
	TArray<ACustomerAI*> pickupCustomers;

	/** [자동 보고] 픽업 존에서 음식이 준비되었다고 보고받았을 때 호출됩니다. */
	void OnFoodPlacedInZone(APickupZone* Zone);
	
	UFUNCTION()
	void UpdateWaitingPosition();
	UFUNCTION()
	void CallNextCustomerFromWandering();

	// 테스트용 임시변수
public:
	UPROPERTY(EditAnywhere, Category = "Spawn")
	bool bRespawn = true;
};
