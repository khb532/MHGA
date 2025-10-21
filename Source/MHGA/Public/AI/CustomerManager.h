// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "CustomerManager.generated.h"

// ADDED: APickupZone 이라는 클래스가 있다고 컴파일러에게 미리 알려줍니다.

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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 손님 스폰 관리
	// 손님 스폰 타이머 핸들
	FTimerHandle spawnTimer;
	// 손님 스폰 최소 시간
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float minTime = 1;
	// 손님 스폰 최대 시간
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float maxTime = 5;
	
	// 스폰할 수 있는 손님의 최대 인원수
	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 MaxSpawnedCustomers = 10;

	// 현재 스폰되어 있는 손님의 수
	UPROPERTY(VisibleAnywhere, Category = "Spawn")
	int32 CurrentSpawnedCustomers = 0;

	// 손님이 가게를 나갈 때 호출되는 함수
	void OnCustomerExited();
	
	// 손님 스폰 포인트
	UPROPERTY(EditAnywhere, Category = "Spawn")
	class ATargetPoint* spawnPoint;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class ACustomerAI> spawnFactory;

	// 손님을 스폰하는 함수
	void SpawnCustomer();
	

	// -- 손님 줄세우기--
	// 주문 대기열 위치
	UPROPERTY(EditAnywhere, Category = "Waiting Order")
	TArray<ATargetPoint*> waitingPoints;
	// 픽업 위치
	UPROPERTY(EditAnywhere, Category = "Waiting Order")
	TArray<ATargetPoint*> pickupPoints;
	
	// 줄에서 대기중인 손님열
	UPROPERTY(EditAnywhere, Category = "Waiting Order", Replicated)
	TArray<ACustomerAI*> waitingCustomers;
	// 줄 밖에서 대기중인 손님열
	UPROPERTY(EditAnywhere, Category = "Waiting Order", Replicated)
	TArray<ACustomerAI*> wanderingCustomers;
	// 줄 밖에서 대기중인 손님열
	// UPROPERTY(EditAnywhere, Category = "Waiting Order")
	// TArray<ACustomerAI*> pickupCustomers;
	//
	// /** [자동 보고] 픽업 존에서 음식이 준비되었다고 보고받았을 때 호출됩니다. */
	// void OnFoodPlacedInZone(class APickupZone* Zone);
	
	UFUNCTION()
	class ATargetPoint* RequestWaitingPoint(ACustomerAI* customer);
	UFUNCTION()
	class ATargetPoint* RequestPickupPoint();
	UFUNCTION()
	class ATargetPoint* RequestExitPoint();

	UFUNCTION()
	void OnCustomerFinished(ACustomerAI* customer);
	
	UFUNCTION()
	void UpdateWaitingPosition();
	UFUNCTION()
	void CallNextCustomerFromWandering();

	// 테스트용 임시변수
public:
	UPROPERTY(EditAnywhere, Category = "Spawn")
	bool bRespawn = true;
};
