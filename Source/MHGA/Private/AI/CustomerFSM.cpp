// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomerFSM.h"

#include "AIController.h"
#include "BurgerData.h"
#include "CustomerUI.h"
#include "MHGAGameState.h"
#include "NavigationSystem.h"
#include "AI/CustomerAI.h"
#include "AI/CustomerManager.h"
#include "Blueprint/UserWidget.h"
#include "Counter/CounterPOS.h"
#include "Engine/TargetPoint.h"
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
	manager = Cast<ACustomerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass()));
	AIController = Cast<AAIController>(me->GetController());

	EnterStore();

}


// Called every frame
void UCustomerFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// const UEnum* EnumPtr = StaticEnum<EAIState>();
	// if (EnumPtr)
	// {
	// 	// GetValueAsString은 "EAIState::GoingToLine" 형태의 전체 이름을 반환합니다.
	// 	FString EnumString = EnumPtr->GetValueAsString(CurrentState);
	//
	// 	// (선택 사항) "EAIState::" 부분을 제거하여 "GoingToLine"만 남기면 더 깔끔합니다.
	// 	EnumString.RemoveFromStart(TEXT("EAIState::"));
	//
	// 	UE_LOG(LogTemp, Warning, TEXT("현재 상태: %s"), *EnumString);
	// 	
	// }
	
	if (CurrentState == EAIState::GoingToLine)
	{
		if (FVector::Dist2D(me->GetActorLocation() , manager->waitingPoints[0]->GetActorLocation()) <= 100)
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
	if (CurrentState == EAIState::Wandering && NewState != EAIState::Wandering)
	{
		GetWorld()->GetTimerManager().ClearTimer(wanderTimerHandle);
	}

	CurrentState = NewState;

	switch (CurrentState)
	{
	case EAIState::GoingToLine:
		{
			// 줄서기 위치까지 이동
			ATargetPoint* orderTarget = manager->RequestWaitingPoint(me);
			if (orderTarget)
			{
				MoveToTarget(orderTarget);
			}
			else if (orderTarget == nullptr && CurrentState != EAIState::Ordering)
			{
				SetState(EAIState::Wandering);
			}
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
			UE_LOG(LogTemp, Error, TEXT("StartWandering"));
			StartWandering();
			break;
		}

	case EAIState::Ordering:
		{
			UE_LOG(LogTemp, Warning, TEXT("주문중"))
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
	// 배회를 시작할 때, MoveToRandomLocation 함수를 "즉시" 한 번 호출하고,
	// 그 후 3~5초마다 반복해서 호출하도록 타이머를 설정합니다.
	// 마지막 파라미터 true가 반복, -1.f는 첫 호출 지연 없음(즉시 실행)을 의미합니다.
	
	GetWorld()->GetTimerManager().SetTimer(
		wanderTimerHandle, 
		this, 
		&UCustomerFSM::MoveToRandomLocation, 
		FMath::RandRange(3.0f, 5.0f), 
		true, 
		0.0f
	);
	
	// auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	//
	// auto result = AIController->MoveToLocation(randomPos);
	// if (result == EPathFollowingRequestResult::Type::AlreadyAtGoal || result == EPathFollowingRequestResult::Failed)
	// {
	// 	GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	// }
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


void UCustomerFSM::MoveToRandomLocation()
{
	if (GetOwner() && GetRandomPositionInNavMesh(GetOwner()->GetActorLocation(), 500, randomPos))
	{
		if (AIController)
		{
			AIController->MoveToLocation(randomPos);
		}
	}
}

void UCustomerFSM::StartOrder()
{
	AMHGAGameState* gs = Cast<AMHGAGameState>(GetWorld()->GetGameState());

	gs->GetCounter()->ServerRPC_SetCustomer(me);
	
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
	if (CurrentState == EAIState::Ordering)
	{
		if (manager)
		{
		manager->OnCustomerFinished(me);
		SetState(EAIState::WaitingForFood);
			UE_LOG(LogTemp, Error, TEXT("주문 완료, 어슬렁"));
			
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FinishOrder 실패: CustomerManager 변수가 할당되지 않았습니다!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("????"));
	}
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

void UCustomerFSM::MoveToTarget(const ATargetPoint* target)
{
	if (target)
	{
		AIController->MoveToLocation(target->GetActorLocation());
	}
}
