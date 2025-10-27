// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BurgerData.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "DialogueData.h"
#include "Navigation/PathFollowingComponent.h"
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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	class AAIController* AIController;
	
public:
	// == 핵심 데이터 ==
	UPROPERTY()
	class ACustomerAI* me;
	UPROPERTY()
	class ACustomerManager* manager;

	// 손님의 현재 상태
	UPROPERTY(EditAnywhere, Category = "AI State", ReplicatedUsing = OnRep_StateChange)
	EAIState curState = EAIState::None;
	// 손님의 현재 성격
	UPROPERTY(VisibleInstanceOnly, Category = "AI State", Replicated)
	ECustomerPersonality personality = ECustomerPersonality::Standard;



	
	/** 특별 손님이 등장할 확률 (0.0 ~ 1.0 사이 값) */
	UPROPERTY(EditDefaultsOnly, Category = "AI State", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SpecialCustomerChance = 0.01f; // 기본값 5%
	/**
	 * 선택된 메쉬 인덱스. -1 = 특별 손님, 0+ = 일반 손님 메쉬 배열 인덱스
	 * 클라이언트는 이 값을 복제받아 AI(몸)의 메쉬를 갱신합니다.
	 */
	UPROPERTY(VisibleInstanceOnly, Category = "AI Visuals", ReplicatedUsing = OnRep_MeshIndex)
	int32 SelectedMeshIndex = 0;
	UFUNCTION()
	void OnRep_MeshIndex();
	


	
	void SetState(EAIState NewState);
	UFUNCTION()
	void OnRep_StateChange();
	UFUNCTION()
	void HandleStateEnter(EAIState state);
	
	// 대사 데이터 테이블
	UPROPERTY(EditAnywhere, Category = "AI Dialogue")
	UDataTable* MenuDialogueTable;
	// 주문 상태 진입 시 생성되어 저장되는 실제 대사
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "AI Order", ReplicatedUsing = OnRep_Dialogue)
	FText curDialogue;
	// UI가 이 변수를 쉽게 가져갈 수 있도록 Getter 함수
	UFUNCTION(BlueprintPure, Category = "AI | Dialogue")
	FText GetCurrentDialogue() const { return curDialogue; }
	UFUNCTION()
	void OnRep_Dialogue();
	
	// 주문한 메뉴
	UPROPERTY(EditAnywhere, Category = "AI Order", Replicated, ReplicatedUsing = OnRep_Order)
	EBurgerMenu orderedMenu;
	// 주문한 수량
	UPROPERTY(VisibleInstanceOnly, Category = "AI Order")
	int32 orderQuantity = 1;
	// 주문 대기 시간
	UPROPERTY(VisibleInstanceOnly, Category = "AI Order")
	float maxOrderTime = 10.f;
	float orderTimer = 0.f;
	// 음식 대기 시간
	UPROPERTY(EditAnywhere, Category = "AI Order")
	float maxWaitTime = 30.f;
	float waitingTimer = 0.f;

	// == 위치정보 ==
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	class ATargetPoint* orderTarget;	// 주문 위치

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	class ATargetPoint* pickupTarget;	// 음식 수령 위치

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	class ATargetPoint* exitTarget;		// 퇴장 위치

public:
	// 주문 시작 함수
	UFUNCTION(Category= "AI Order")
	void StartOrder();
	UFUNCTION()
	void OnRep_Order();
	// 주문한 메뉴를 텍스트로 반환하는 함수
	UFUNCTION(BlueprintPure, Category = "AI Order")
	FText GetOrderedMenuAsText();
	
	UFUNCTION()
	void FinishOrder();
	
	UPROPERTY(EditAnywhere)
	class APickupZone* MyPickupZone;
	
	UFUNCTION()
	void EnterStore();
	UFUNCTION(Server, Reliable)
	void Server_EnterStore();
	
	UFUNCTION()
	void StartWandering();
	UFUNCTION()
	void StopWandering();
	FVector randomPos;
	bool GetRandomPositionInNavMesh(const FVector& centerPos, const float radius, FVector& dest);
	// 배회 행동을 주기적으로 실행할 타이머 핸들
	FTimerHandle wanderTimerHandle;
	// 배회 상태일 때 다음 목적지로 이동시키는 함수
	void MoveToRandomLocation();
	
	UFUNCTION()
	void CallToPickup();
	UFUNCTION()
	void WaitingForPickup();

	void CheckAndTakeFood();

	UFUNCTION()
	void ExitStore();

	UFUNCTION()
	void OnCalledToPickup();
	
	UFUNCTION()
	void MoveToTarget(const ATargetPoint* target);
	
	void OnMoveToTargetCompleted(FAIRequestID id, const FPathFollowingResult& result);
};
