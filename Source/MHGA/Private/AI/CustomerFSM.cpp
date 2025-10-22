// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomerFSM.h"

#include "AIController.h"
#include "BurgerData.h"
#include "public/AI/CustomerUI.h"
#include "MHGAGameState.h"
#include "NavigationSystem.h"
#include "AI/CustomerAI.h"
#include "AI/CustomerManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Counter/CounterPOS.h"
#include "Counter/PickupZone.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCustomerFSM::UCustomerFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	waitingTimer = 0.f;
}

// Called when the game starts
void UCustomerFSM::BeginPlay()
{
	Super::BeginPlay();
	me = Cast<ACustomerAI>(GetOwner());
	manager = Cast<ACustomerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass()));
	AIController = Cast<AAIController>(me->GetController());

	// ★★★ 핵심 수정: BeginPlay에서 픽업 존을 직접 찾아 저장합니다 ★★★
	MyPickupZone = Cast<APickupZone>(UGameplayStatics::GetActorOfClass(GetWorld(), APickupZone::StaticClass()));
	if (MyPickupZone == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("FSM이 레벨에서 PickupZone을 찾을 수 없습니다!"));
	}

	personality = static_cast<ECustomerPersonality>(FMath::RandRange(0, 3));
	EnterStore();
	
}


void UCustomerFSM::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCustomerFSM, curState);
	DOREPLIFETIME(UCustomerFSM, orderedMenu);
	DOREPLIFETIME(UCustomerFSM, curDialogue);
}

// Called every frame
void UCustomerFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner()->HasAuthority()) return;
	
	// 지속되는 상태만 처리
	if (curState == EAIState::WaitingForFood)
	{
		waitingTimer += DeltaTime;
		if (waitingTimer > maxWaitTime)
		{
			UE_LOG(LogTemp, Warning, TEXT("너무 오래걸려서 돌아갔습니다"));
			me->ShowReputationText(false);
			SetState(EAIState::Exit);
		}
	}


	if (curState == EAIState::GoingToPickup)
	{
		if (FVector::Dist2D(me->GetActorLocation() , manager->pickupPoints[0]->GetActorLocation()) <= 100)
		{
			SetState(EAIState::WaitingForPickup);
		}
	}

	if (curState == EAIState::WaitingForFood)
	{
		
	}

	if (curState == EAIState::Exit)
	if (FVector::Dist2D(me->GetActorLocation() , manager->spawnPoint->GetActorLocation()) <= 100)
	{
		me->Destroy();
	}
	
}


void UCustomerFSM::SetState(EAIState NewState)
{
	if (!GetOwner()->HasAuthority()) return;
	
	// 이전과 같은 상태라면 함수에서 나가기
	if (curState == NewState) return;
	
	curState = NewState;

	HandleStateEnter(curState);
	
}

void UCustomerFSM::OnRep_StateChange()
{
	if (GetOwner()->HasAuthority()) return;

	HandleStateEnter(curState);

	if (me)
	{
		if (curState == EAIState::Ordering)
		{
			me->ShowOrderUI();
		}
		else
		{
			me->HideOrderUI();
		}
	}

}

void UCustomerFSM::OnRep_Dialogue()
{
	if (me && me->customerWidget)
	{
		if (UCustomerUI* orderWidget = Cast<UCustomerUI>(me->customerWidget->GetUserWidgetObject()))
		{
			orderWidget->Event_SetOrderText(curDialogue);
		}
	}
}

void UCustomerFSM::HandleStateEnter(EAIState state)
{	
	StopWandering();
	
	switch (state)
	{
	case EAIState::GoingToLine:
		{
			// 줄서기 위치까지 이동
			if (!GetOwner()->HasAuthority()) return;
			
			if (orderTarget)
			{
				MoveToTarget(orderTarget);
				UE_LOG(LogTemp, Warning, TEXT("주문하러 이동중"))
				AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UCustomerFSM::OnMoveToTargetCompleted);
			}
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
			if (GetOwner()->HasAuthority())
			{
				UE_LOG(LogTemp, Warning, TEXT("주문중"))
				StartOrder();
			}
			break;
		}

	case EAIState::WaitingForFood:
		{
			StartWandering();
			break;
		}
		
	case EAIState::GoingToPickup:
		{
			pickupTarget = manager->RequestPickupPoint();
			if (pickupTarget)
			{
				MoveToTarget(pickupTarget);
				UE_LOG(LogTemp, Log, TEXT("픽업하러 이동중"));
			}
			break;
		}
		
	case EAIState::Exit:
		{
			ExitStore();
			break;
		}
	case EAIState::None:
		break;
		
	case EAIState::WaitingForPickup:
		CheckAndTakeFood();
		break;
	}
}

void UCustomerFSM::EnterStore()
{
	Server_EnterStore();
}

void UCustomerFSM::Server_EnterStore_Implementation()
{
	// 가게 입장시 먼저 대기열이 있는지 확인
	orderTarget = manager->RequestWaitingPoint(me);
	// 대기열에 빈자리가 있다면
	if (orderTarget)
	{
		SetState(EAIState::GoingToLine);
	}
	// 대기열에 빈자리가 없다면
	else
	{
		SetState(EAIState::Wandering);
	}
}

void UCustomerFSM::OnCalledToPickup()
{
	if (curState == EAIState::WaitingForFood)
	{
		SetState(EAIState::GoingToPickup);
	}
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
}

void UCustomerFSM::StopWandering()
{
	GetWorld()->GetTimerManager().ClearTimer(wanderTimerHandle);
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
	if (!IsValid(AIController) || !GetOwner()) return;
	
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
	// 혹시 모르는 방어 코드(지워도 됨)
	if (!GetOwner()->HasAuthority()) return;
	
	AMHGAGameState* gs = Cast<AMHGAGameState>(GetWorld()->GetGameState());
	gs->GetCounter()->ServerRPC_SetCustomer(me);
	
	// 랜덤으로 선택할 메뉴의 최소인덱스와 최대인덱스를 정수로 가져온다
	int32 MinMenuIndex = static_cast<int32>(EBurgerMenu::BigMac);
	int32 MaxMenuIndex = static_cast<int32>(EBurgerMenu::Shrimp);
	
	// 해당 범위 내에서 랜덤 정수를 선택한다
	int32 RandomMenuIndex = FMath::RandRange(MinMenuIndex, MaxMenuIndex);

	// 랜덤으로 선택된 정수를 enum 타입으로 변환해 변수에 저장한다
	orderedMenu = static_cast<EBurgerMenu>(RandomMenuIndex);

	// UEnum*을 찾아 enum 값을 문자열로 변환합니다.
	const UEnum* BurgerEnum = FindObject<UEnum>(nullptr, TEXT("/Script/MHGA.EBurgerMenu"), true);
	if (BurgerEnum)
	{
		FString EnumAsString = BurgerEnum->GetNameStringByValue(static_cast<int64>(orderedMenu));
		UE_LOG(LogTemp, Warning, TEXT("주문 메뉴 결정: %s"), *EnumAsString);
	}
	
	curDialogue = GetOrderedMenuAsText();
	orderQuantity = FMath::RandRange(1, 3);
	UE_LOG(LogTemp, Error, TEXT("%s"), *curDialogue.ToString());
	
	me->ShowOrderUI();
}

void UCustomerFSM::OnRep_Order()
{
	me->ShowOrderUI();
}

FText UCustomerFSM::GetOrderedMenuAsText()
{
	if (!MenuDialogueTable) return FText::GetEmpty();
	
	// Enum 값(EBurgerMenu::BigMac)을 FName("BigMac")으로 변환
    // (데이터 테이블의 Row Name과 일치해야 함)
    const FString EnumString = StaticEnum<EBurgerMenu>()->GetNameStringByValue(static_cast<int64>(orderedMenu));
    const FName RowName = FName(*EnumString);
 
    // 데이터 테이블에서 해당 메뉴의 행 찾기
    static const FString ContextString(TEXT("GetOrderedMenuAsText"));
    FOrderDialogue* DialogueRow = MenuDialogueTable->FindRow<FOrderDialogue>(RowName, ContextString);
 
    if (!DialogueRow)
    {
        UE_LOG(LogTemp, Warning, TEXT("DT_MenuDialogue에서 %s 행을 찾을 수 없습니다."), *RowName.ToString());
        return FText::GetEmpty();
    }
 
    // 4. 손님의 성격(Personality)에 따라 사용할 대사 배열(Variations)을 선택
    const TArray<FText>* Variations = nullptr; // 원본 배열을 가리킬 포인터
    switch (personality)
    {
    case ECustomerPersonality::Polite:
        Variations = &DialogueRow->Polite_Variations;
        break;
    case ECustomerPersonality::Rude:
        Variations = &DialogueRow->Rude_Variations;
        break;
    case ECustomerPersonality::Impatient:
        Variations = &DialogueRow->Impatient_Variations;
        break;
    case ECustomerPersonality::Standard:
    default:
        Variations = &DialogueRow->Standard_Variations;
        break;
    }
 
    // 5. 선택된 배열에서 무작위 대사 하나를 선택
    if (!Variations || Variations->Num() == 0)
    {
        // 해당 성격에 대사가 없으면 Standard로 대체 시도
        Variations = &DialogueRow->Standard_Variations;
        if (!Variations || Variations->Num() == 0)
        {
             UE_LOG(LogTemp, Warning, TEXT("%s 행에 유효한 대사가 없습니다."), *RowName.ToString());
             return FText::GetEmpty(); // 대체 대사도 없으면 포기
        }
    }
 
    // 무작위 인덱스 선택
    const int32 RandomIndex = FMath::RandRange(0, Variations->Num() - 1);
    const FText FormatTemplate = (*Variations)[RandomIndex]; // 포맷팅할 원본 FText (예: "{MenuName} {Quantity}개 주세요.")
 
    //  FText::Format을 사용하여 대사 포맷팅
    FFormatNamedArguments Args;
    Args.Add(TEXT("MenuName"), DialogueRow->MenuDisplayName); 
    Args.Add(TEXT("Quantity"), orderQuantity);             
 
    return FText::Format(FormatTemplate, Args);
}

void UCustomerFSM::FinishOrder()
{
	if (curState == EAIState::Ordering)
	{
		if (manager)
		{
			manager->OnCustomerFinished(me);
			me->HideOrderUI();
			SetState(EAIState::WaitingForFood);
			UE_LOG(LogTemp, Error, TEXT("주문 완료"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FinishOrder 실패: CustomerManager 변수가 할당되지 않았습니다!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("주문 실패????"));
	}
}


void UCustomerFSM::CallToPickup()
{
	// 음식을 기다리거나 배회하는 상태일 때만 호출에 응답합니다.
	if ((curState == EAIState::WaitingForFood || curState == EAIState::Wandering))
	{
		SetState(EAIState::GoingToPickup);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("픽업하러 못감"));
	}
}

void UCustomerFSM::WaitingForPickup()
{
}


void UCustomerFSM::CheckAndTakeFood()
{
	if (!IsValid(MyPickupZone))
	{
		UE_LOG(LogTemp, Log, TEXT("몬진 모르겠는데 암튼 오류임"));
	return;
	}

	if (MyPickupZone->HasFood())
	{
		// 픽업 존에서 가져온 액터를 AHamburger로 형변환합니다.
		AHamburger* TakenHamburger = Cast<AHamburger>(MyPickupZone->TakeFood());
		
		if (IsValid(TakenHamburger))
		{
			// --- 1. 주문한 메뉴(enum)를 FString으로 변환 ---
			FString OrderedMenuName;
			UEnum* BurgerEnum = StaticEnum<EBurgerMenu>();
			if (BurgerEnum)
			{
				// GetNameStringByValue는 "BigMac"과 같이 깔끔한 이름을 반환합니다.
				OrderedMenuName = BurgerEnum->GetNameStringByValue(static_cast<int64>(orderedMenu));
			}

			// --- 2. 햄버거 액터에서 FString 이름을 가져옴 ---
			FString TakenBurgerName = TakenHamburger->GetBurgerName();

			// --- 3. 두 FString을 비교 ---
			if (OrderedMenuName == TakenBurgerName)
			{
				me->ShowReputationText(true);
				UE_LOG(LogTemp, Log, TEXT("주문한 메뉴와 동일! 만족!"));
				// TODO: 평점 올리는 로직
				SetState(EAIState::Exit);
			}
			else
			{
				me->ShowReputationText(false);
				UE_LOG(LogTemp, Warning, TEXT("다른 메뉴를 받음! 주문: %s, 받은 것: %s"), *OrderedMenuName, *TakenBurgerName);
				// TODO: 평점 내리는 로직
				SetState(EAIState::Exit);
			}

			TakenHamburger->Destroy();
			SetState(EAIState::Exit);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("픽업대에 도착했지만 음식이 없습니다. 다시 대기합니다..."));
		SetState(EAIState::Wandering);
	}
}

void UCustomerFSM::ExitStore()
{
	exitTarget = manager->RequestExitPoint();
	if (exitTarget)
	{
		MoveToTarget(exitTarget);
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		if (manager)
		{
			manager->OnCustomerExited();
		}
	}
}

void UCustomerFSM::MoveToTarget(const ATargetPoint* target)
{
	if (target)
	{
		AIController->MoveToLocation(target->GetActorLocation());
	}
}

void UCustomerFSM::OnMoveToTargetCompleted(FAIRequestID id, const FPathFollowingResult& result)
{
	// 서버에서만 실행되게
	if (!GetOwner()->HasAuthority()) return;
	// 줄서러 가는중 or 줄서기중이 아니면 함수 탈출
	if (curState != EAIState:: GoingToLine && curState != EAIState::WaitingInLine) return;

	// 목적지 도착 성공시
	if (result.Code == EPathFollowingResult::Success)
	{
		if (orderTarget && manager && orderTarget == manager->waitingPoints[0])
		{
			UE_LOG(LogTemp, Warning, TEXT("도착, 주문 시작"));
			SetState(EAIState::Ordering);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("도착, 줄서서 대기"));
			SetState(EAIState::WaitingInLine);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("이동 실패, 다시 대기열 요청"));
		EnterStore();
	}
}

