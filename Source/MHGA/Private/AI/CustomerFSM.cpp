// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomerFSM.h"

#include "AIController.h"
#include "BurgerData.h"
#include "MHGAGameMode.h"
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
}

// Called when the game starts
void UCustomerFSM::BeginPlay()
{
	Super::BeginPlay();
	me = Cast<ACustomerAI>(GetOwner());
	AIController = Cast<AAIController>(me->GetController());
	manager = Cast<ACustomerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass()));
	MyPickupZone = Cast<APickupZone>(UGameplayStatics::GetActorOfClass(GetWorld(), APickupZone::StaticClass()));
	if (MyPickupZone == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("FSM이 레벨에서 PickupZone을 찾을 수 없습니다!"));
	}

	if (GetOwner()->HasAuthority())
	{
		// 1. 특별 손님 여부 결정
		if (FMath::FRand() < SpecialCustomerChance)
		{
			// --- 특별 손님 ---
			personality = ECustomerPersonality::Special_VIP;
			SelectedMeshIndex = -1; // -1은 특별 손님 메쉬를 의미
		}
		else
		{
			// --- 일반 손님 ---
            
			// A. 성격 랜덤 결정 (Special_VIP 제외)
			int32 NumRegularPersonalities = static_cast<int32>(ECustomerPersonality::Special_VIP);
			personality = static_cast<ECustomerPersonality>(FMath::RandRange(0, NumRegularPersonalities - 1));

			// B. 메쉬 랜덤 결정 (AI의 배열 사용)
			if (me && me->regularVisuals.Num() > 0)
			{
				SelectedMeshIndex = FMath::RandRange(0, me->regularVisuals.Num() - 1);
			}
			else
			{
				SelectedMeshIndex = 0; // 혹시 모를 폴백
				if(me && me->regularVisuals.Num() == 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("FSM BeginPlay: ACustomerAI의 RegularCustomerMeshes 배열이 비어있습니다!"));
				}
			}
		}

		// ★★★ 중요: OnRep은 서버에서 호출되지 않으므로,
		// 서버의 AI(몸)도 직접 메쉬를 업데이트해줘야 합니다.
		if (me)
		{
			me->UpdateVisuals(SelectedMeshIndex);
		}
	}
	
	// personality = static_cast<ECustomerPersonality>(FMath::RandRange(0, 3));
	EnterStore();
	
}


void UCustomerFSM::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCustomerFSM, curState);
	DOREPLIFETIME(UCustomerFSM, orderedMenu);
	DOREPLIFETIME(UCustomerFSM, curDialogue);

	DOREPLIFETIME(UCustomerFSM, personality);
	DOREPLIFETIME(UCustomerFSM, SelectedMeshIndex);
}

// Called every frame
void UCustomerFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner()->HasAuthority()) return;
	
	// 지속되는 상태만 처리
	if (curState == EAIState::Ordering)
	{
		orderTimer += DeltaTime;
		if (orderTimer > maxOrderTime)
		{
			AMHGAGameMode* gm = GetWorld()->GetAuthGameMode<AMHGAGameMode>();
			gm->ReportScoreChanged(EScoreChangeReason::SlowOrder, gm->penaltySlowOrder);

			UE_LOG(LogTemp, Warning, TEXT("주문이 너무 오래걸려서 돌아갔습니다"));
			me->ShowScoreFeedback(EScoreChangeReason::SlowOrder);
			manager->OnCustomerFinished(me);
			SetState(EAIState::Exit);
		}
	}
	if (curState == EAIState::WaitingForFood)
	{
		waitingTimer += DeltaTime;
		if (waitingTimer > maxWaitTime)
		{
			AMHGAGameMode* gm = GetWorld()->GetAuthGameMode<AMHGAGameMode>();
			gm->ReportScoreChanged(EScoreChangeReason::SlowCook, gm->penaltySlowCook);
			
			UE_LOG(LogTemp, Warning, TEXT("음식이 너무 오래걸려서 돌아갔습니다"));
			me->ShowScoreFeedback(EScoreChangeReason::SlowCook);
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

void UCustomerFSM::OnRep_MeshIndex()
{
	if (IsValid(me)) // <- 이 코드가 Stale Pointer를 안전하게 걸러냅니다.
	{
		me->UpdateVisuals(SelectedMeshIndex);
	}
}



void UCustomerFSM::SetState(EAIState NewState)
{
	// 서버에서만 실행
	if (!GetOwner()->HasAuthority()) return;
	
	// 이전과 같은 상태라면 함수에서 나가기
	if (curState == NewState) return;
	
	curState = NewState;
	HandleStateEnter(curState);
	
	if (me)
	{
		if (curState == EAIState::Ordering)
		{
			me->ShowCustomerDialogue();
		}
	}
}

void UCustomerFSM::OnRep_StateChange()
{
	// 클라이언트에서만 실행
	// if (GetOwner()->HasAuthority()) return;
	//
	// if (me)
	// {
	// 	if (curState == EAIState::Ordering)
	// 	{
	// 		me->ShowCustomerDialogue();
	// 	}
	// }

}

void UCustomerFSM::OnRep_Dialogue()
{
	if (me && me->customerWidget)
	{
		if (UCustomerUI* orderWidget = Cast<UCustomerUI>(me->customerWidget->GetUserWidgetObject()))
		{
			me->ShowCustomerDialogue();
		}
	}
}

void UCustomerFSM::HandleStateEnter(EAIState state)
{	
	if (!GetOwner()->HasAuthority()) return;
	StopWandering();
	
	switch (state)
	{
	case EAIState::GoingToLine:
		{
			// 줄서기 위치까지 이동			
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

	if (personality == ECustomerPersonality::Special_VIP)
	{
		orderedMenu = EBurgerMenu::WrongBurger;
	}
	// UEnum을 찾아 enum 값을 문자열로 변환
	const UEnum* BurgerEnum = FindObject<UEnum>(nullptr, TEXT("/Script/MHGA.EBurgerMenu"), true);
	if (BurgerEnum)
	{
		FString EnumAsString = BurgerEnum->GetNameStringByValue(static_cast<int64>(orderedMenu));
		UE_LOG(LogTemp, Warning, TEXT("주문 메뉴 결정: %s"), *EnumAsString);
	}
	
	orderQuantity = FMath::RandRange(1, 3);
	curDialogue = GetOrderedMenuAsText();
	UE_LOG(LogTemp, Error, TEXT("현재 대사 : %s, 현재 수량 : %d"), *curDialogue.ToString(), orderQuantity);
}

void UCustomerFSM::OnRep_Order()
{
	me->ShowCustomerDialogue();
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
    case ECustomerPersonality::Special_VIP:
    	Variations = &DialogueRow->Special_Variations;
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
			me->HideCustomerDialogue();
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
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (!IsValid(MyPickupZone)) return;

	if (MyPickupZone->HasFood())
	{
		// 픽업 존에서 가져온 액터를 AHamburger로 형변환합니다.
		AHamburger* TakenHamburger = Cast<AHamburger>(MyPickupZone->TakeFood());
		
		if (IsValid(TakenHamburger))
		{
			AMHGAGameMode* gm = GetWorld()->GetAuthGameMode<AMHGAGameMode>();
			// --- 1. 주문한 메뉴(enum)를 FString으로 변환 ---
			FString OrderedMenuName;
			UEnum* BurgerEnum = StaticEnum<EBurgerMenu>();
			if (BurgerEnum)
			{
				// GetNameStringByValue는 "BigMac"과 같이 깔끔한 이름을 반환합니다.
				OrderedMenuName = BurgerEnum->GetNameStringByValue(static_cast<int64>(orderedMenu));
			}

			// 햄버거 액터에서 FString 이름을 가져옴
			FString TakenBurgerName = TakenHamburger->GetBurgerName();

			// 두 FString을 비교
			if (OrderedMenuName == TakenBurgerName)
			{
				// 같은 값이면 평점 올리기
				gm->ReportScoreChanged(EScoreChangeReason::CorrectFood, gm->bonusCorrectFood);
				me->ShowScoreFeedback(EScoreChangeReason::CorrectFood);
				UE_LOG(LogTemp, Log, TEXT("주문한 메뉴와 동일! 만족!"));
				manager->OnCustomerFinished(me);
				SetState(EAIState::Exit);
			}
			else
			{
				// 다른 음식이면 평점 깎기
				gm->ReportScoreChanged(EScoreChangeReason::WrongFood, gm->penaltyWrongFood);
				me->ShowScoreFeedback(EScoreChangeReason::WrongFood);
				UE_LOG(LogTemp, Warning, TEXT("다른 메뉴를 받음! 주문: %s, 받은 것: %s"), *OrderedMenuName, *TakenBurgerName);
				manager->OnCustomerFinished(me);
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
	// if (result.Code == EPathFollowingResult::Success)
	if (result.IsSuccess())
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

