// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BurgerData.h"
#include "Components/ActorComponent.h"
#include "CustomerFSM.generated.h"

class ATargetPoint;
class APickupZone;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	None,			// 초기 상태
	GoingToLine,	// 줄 서러 이동 중
	WaitingInLine,	// 줄 서서 대기 중
	Wandering,		// (줄이 꽉 찼을 때) 배회 중
	Ordering,		// 주문 중
	WaitingForFood,	// 음식 대기 중
	GoingToPickup,	// 음식 받으러 이동 중
	WaitingForPickup,	// 음식 대기중
	Exit			// 매장 퇴장
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MHGA_API UCustomerFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCustomerFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
private:
	class AAIController* AIController;
	
public:
	// == 핵심 데이터 ==
	UPROPERTY()
	class ACustomerAI* me;
	UPROPERTY()
	class ACustomerManager* manager;
	
	UPROPERTY(EditAnywhere, Category = "AI State")
	EAIState CurrentState = EAIState::None;	// 현재 AI의 상태

	UPROPERTY(EditAnywhere, Category = "AI State")
	FString DesiredMenu;	// 손님이 주문한 메뉴

	UPROPERTY(EditAnywhere, Category = "AI State")
	float maxWaitTime = 30.f;		// 최대 대기 시간
	
	float waitingTimer;		// 대기시간 측정 타이머

	// == 위치정보 ==
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	class ATargetPoint* orderTarget;	// 주문 위치

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	class ATargetPoint* LineTarget;		// 대기열 위치

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	class ATargetPoint* pickupTarget;	// 음식 수령 위치

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	class ATargetPoint* ExitTarget;		// 퇴장 위치

public:
	// == 함수 ==
	void SetState(EAIState NewState);

	UPROPERTY(EditAnywhere, Category="AI Navigation")
	TArray<ATargetPoint*> targetPoints;
	
	UPROPERTY(EditAnywhere)
	APickupZone* MyPickupZone;
	
	UFUNCTION()
	void EnterStore();
	
	UFUNCTION()
	void StartWandering();
	FVector randomPos;
	bool GetRandomPositionInNavMesh(const FVector& centerPos, const float radius, FVector& dest);
	// 배회 행동을 주기적으로 실행할 타이머 핸들
	FTimerHandle wanderTimerHandle;
	// 배회 상태일 때 다음 목적지로 이동시키는 함수
	void MoveToRandomLocation();

	// 주문 시작 함수
	UFUNCTION(Category= "AI Order")
	void StartOrder();
	// 주문한 메뉴 저장하는 변수
	UPROPERTY(EditAnywhere, Category = "AI Order")
	EBurgerMenu OrderedMenu;
	// 주문한 메뉴를 텍스트로 반환하는 함수
	UFUNCTION(BlueprintPure, Category = "AI Order")
	FText GetOrderedMenuAsText();
	
	UFUNCTION()
	void FinishOrder();
	
	UFUNCTION()
	void CallToPickup();
	UFUNCTION()
	void WaitingForPickup();

public:
	void CheckAndTakeFood();

	UFUNCTION()
	void ExitStore();

	UFUNCTION()
	void OnCalledToPickup();
	UFUNCTION()
	void ReceiveFood(const FString& receivedFood);
	
	UFUNCTION()
	void MoveToTarget(const ATargetPoint* target);
		
};
