// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomerFSM.h"

#include "AIController.h"
#include "AI/CustomerAI.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCustomerFSM::UCustomerFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	StateTimer = 0.f;
}


// Called when the game starts
void UCustomerFSM::BeginPlay()
{
	Super::BeginPlay();
	me = Cast<ACustomerAI>(GetOwner());

	AIController = Cast<AAIController>(me->GetController());

	FindTarget();
	
	SetState(EAIState::GoingToLine);
}


// Called every frame
void UCustomerFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 지속되는 상태만 처리
	if (CurrentState == EAIState::WaitingForFood)
	{
		StateTimer += DeltaTime;
		if (StateTimer > MaxWaitTime)
		{
			UE_LOG(LogTemp, Warning, TEXT("너무 오래걸려서 돌아갔습니다"))
			SetState(EAIState::Exit);
		}
	}

	if (CurrentState == EAIState::GoingToLine)
	{
		if (FVector::Dist(me->GetActorLocation() , OrderTarget->GetActorLocation()) <= 0)
		{
			SetState(EAIState::Ordering);
		}
	}

	if (CurrentState == EAIState::GoingToPickup)
	{
		if (FVector::Dist(me->GetActorLocation() , PickupTarget->GetActorLocation()) <= 0)
		{
			SetState(EAIState::WaitingForPickup);
		}
	}

	if (CurrentState == EAIState::WaitingForFood)
	{
		
	}
	
}

void UCustomerFSM::SetState(EAIState NewState)
{
	// 이전과 같은 상태라면 함수에서 나가기
	if (CurrentState == NewState)
	{
		return;
	}

	// 타이머 초기화
	if (CurrentState == EAIState::WaitingForFood)
	{
		StateTimer = 0;
	}

	CurrentState = NewState;

	switch (CurrentState)
	{
	case EAIState::GoingToLine:
		{
			// 줄서기 위치까지 이동
			MoveToTarget(OrderTarget);
			break;
		}
		
	case EAIState::WaitingInLine:
		{
			// 이동을 멈추고 가만히 대기
			if (AIController) AIController->StopMovement();
			break;
		}
		
	case EAIState::Wandering:
		{
			StartWandering();
			break;
		}

	case EAIState::Ordering:
		{
			StartOrder();
			break;
		}

	case EAIState::WaitingForFood:
		{
			StartWandering();
			break;
		}
		
	case EAIState::GoingToPickup:
		{
			if (PickupTarget == nullptr) return;
			MoveToTarget(PickupTarget);
			break;
		}
		
	case EAIState::Exit:
		{
			if (ExitTarget == nullptr) return;
			MoveToTarget(ExitTarget);
			break;
		}
	case EAIState::None:
		break;
		
	case EAIState::WaitingForPickup:
		WaitingForPickup();
		break;
	}
}

void UCustomerFSM::FindTarget()
{
	TArray<AActor*> allActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), allActors);
	for (auto target: allActors)
	{
		// 이름이 spawnpoint 녀석을 배열에 추가
		if (target->GetName().Contains(TEXT("Target")))
		{
			targetPoints.Add(target);
		}
	}
	OrderTarget = targetPoints[0];
}


void UCustomerFSM::OnOrderCompleted()
{
	if (CurrentState == EAIState::Ordering)
	{
		SetState(EAIState::WaitingForFood);
	}
}

void UCustomerFSM::OnCalledToPickup()
{
	if (CurrentState == EAIState::WaitingForFood)
	{
		SetState(EAIState::GoingToPickup);
	}
}

void UCustomerFSM::ReceiveFood(const FString& receivedFood)
{
	if (receivedFood == DesiredMenu)
	{
		UE_LOG(LogTemp, Log, TEXT("주문한 메뉴와 동일"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("다른 메뉴를 전달함"));
	}
	SetState(EAIState::Exit);
}

void UCustomerFSM::EnterStore()
{
}

void UCustomerFSM::StartWandering()
{
}

void UCustomerFSM::StartOrder()
{
}

void UCustomerFSM::FinishOrder()
{
}

void UCustomerFSM::CallToPickup()
{
}

void UCustomerFSM::WaitingForPickup()
{
}

void UCustomerFSM::ExitStore()
{
}

void UCustomerFSM::MoveToTarget(const AActor* target)
{
	if (target)
	{
		AIController->MoveToLocation(target->GetActorLocation());
	}
}
