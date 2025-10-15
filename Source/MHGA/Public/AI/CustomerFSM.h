// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomerFSM.generated.h"

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
	
	UPROPERTY(EditAnywhere, Category = "AI State")
	EAIState CurrentState = EAIState::None;	// 현재 AI의 상태

	UPROPERTY(EditAnywhere, Category = "AI State")
	FString DesiredMenu;	// 손님이 주문한 메뉴

	UPROPERTY(EditAnywhere, Category = "AI State")
	float MaxWaitTime = 30.f;		// 최대 대기 시간
	
	float StateTimer;		// 대기시간 측정 타이머

	// == 위치정보 ==
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	class AActor* OrderTarget;	// 주문 위치

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	class AActor* LineTarget;		// 대기열 위치

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	class AActor* PickupTarget;	// 음식 수령 위치

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	class AActor* ExitTarget;		// 퇴장 위치

public:
	// == 함수 ==
	void SetState(EAIState NewState);

	UFUNCTION()
	void FindTarget();
	UPROPERTY(EditAnywhere, Category="AI Navigation")
	TArray<AActor*> targetPoints;
	
	UFUNCTION()
	void EnterStore();
	UFUNCTION()
	void StartWandering();
	UFUNCTION()
	void StartOrder();
	UFUNCTION()
	void FinishOrder();
	UFUNCTION()
	void CallToPickup();
	UFUNCTION()
	void WaitingForPickup();

	UFUNCTION()
	void ExitStore();

	UFUNCTION()
	void OnOrderCompleted();
	UFUNCTION()
	void OnCalledToPickup();
	UFUNCTION()
	void ReceiveFood(const FString& receivedFood);
	
	UFUNCTION()
	void MoveToTarget(const AActor* target);
		
};
