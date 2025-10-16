// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomerFSM.h"

#include "AIController.h"
#include "BurgerData.h"
#include "CustomerUI.h"
#include "NavigationSystem.h"
#include "AI/CustomerAI.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

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

	EnterStore();
}


// Called every frame
void UCustomerFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentState == EAIState::GoingToLine)
	{
		if (FVector::Dist2D(me->GetActorLocation() , OrderTarget->GetActorLocation()) <= 100)
		{
			UE_LOG(LogTemp, Warning, TEXT("주문 시작"))
			SetState(EAIState::Ordering);
		}
	}
	
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
			UE_LOG(LogTemp, Warning, TEXT("주문하러 이동중"))
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
	SetState(EAIState::GoingToLine);
}

void UCustomerFSM::StartWandering()
{
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	auto result = AIController->MoveToLocation(randomPos);
	if (result == EPathFollowingRequestResult::Type::AlreadyAtGoal || result == EPathFollowingRequestResult::Failed)
	{
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

bool UCustomerFSM::GetRandomPositionInNavMesh(const FVector& centerPos, const float radius, FVector& dest)
{
	// 네비게이션 시스템 가져오기
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	// 랜덤 위치 가져오기
	FNavLocation loc;
	bool bResult = ns->GetRandomReachablePointInRadius(centerPos, radius, loc);
	dest = loc.Location;
	return bResult;
}

void UCustomerFSM::StartOrder()
{
	// 랜덤으로 선택할 메뉴의 최소인덱스와 최대인덱스를 정수로 가져온다
	int32 MinMenuIndex = static_cast<int32>(EBurgerMenu::BigMac);
	int32 MaxMenuIndex = static_cast<int32>(EBurgerMenu::Shrimp);
	
	// 해당 범위 내에서 랜덤 정수를 선택한다
	int32 RandomMenuIndex = FMath::RandRange(MinMenuIndex, MaxMenuIndex);

	// 랜덤으로 선택된 정수를 enum 타입으로 변환해 변수에 저장한다
	OrderedMenu = static_cast<EBurgerMenu>(RandomMenuIndex);

	// UEnum*을 찾아 enum 값을 문자열로 변환합니다.
	const UEnum* BurgerEnum = FindObject<UEnum>(nullptr, TEXT("/Script/MHGA.EBurgerMenu"), true);
	if (BurgerEnum)
	{
		FString EnumAsString = BurgerEnum->GetNameStringByValue(static_cast<int64>(OrderedMenu));
		UE_LOG(LogTemp, Warning, TEXT("주문 메뉴 결정: %s"), *EnumAsString);
	}
	
	me->ShowOrderUI();
}

FText UCustomerFSM::GetOrderedMenuAsText()
{
	switch (OrderedMenu)
	{
		case EBurgerMenu::BigMac:
		return NSLOCTEXT("BurgerMenu", "BicMac", "빅맥");
	case EBurgerMenu::BTD:
		return NSLOCTEXT("BurgerMenu", "BTD", "베토디");
	case EBurgerMenu::QPC:
		return NSLOCTEXT("BurgerMenu", "QPC", "쿼파치");
	case EBurgerMenu::Shanghai:
		return NSLOCTEXT("BurgerMenu", "Shanghai", "상하이");
	case EBurgerMenu::Shrimp:
		return NSLOCTEXT("BurgerMenu", "Shrimp", "새우 버거");
	default:
		return FText::GetEmpty();
	}
}

void UCustomerFSM::FinishOrder()
{
	SetState(EAIState::WaitingForFood);
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
