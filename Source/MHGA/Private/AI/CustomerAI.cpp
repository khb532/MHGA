// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomerAI.h"

#include "AIController.h"
#include "public/AI/CustomerUI.h"
#include "AI/CustomerFSM.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "DSP/AudioDebuggingUtilities.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Renderer/Internal/VT/VirtualTextureFeedbackResource.h"

// Sets default values
ACustomerAI::ACustomerAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	fsm = CreateDefaultSubobject<UCustomerFSM>(TEXT("FSM"));
	
	customerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CustomerWidget"));
	customerWidget->SetupAttachment(GetRootComponent());
	
	customerWidget->SetWidgetSpace(EWidgetSpace::World);
	
	customerWidget->SetRelativeLocation(FVector(0, 0, 100));
	customerWidget->SetDrawSize(FVector2D(350, 70));
	customerWidget->SetPivot(FVector2D(0.5f, 0.5f));
	
	customerWidget->SetVisibility(false);
	customerWidget->SetIsReplicated(true);
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bReplicates = true;
	if(fsm) fsm->SetIsReplicated(true); 
	if(customerWidget) customerWidget->SetIsReplicated(true); 
}

// Called when the game starts or when spawned
void ACustomerAI::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACustomerAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 위젯 회전(모든 클라이언트)
	if (customerWidget && customerWidget->IsVisible())
	{
		APlayerCameraManager* camManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (camManager)
		{
			FVector camLocation = camManager->GetCameraLocation();
			FVector widgetLocation = customerWidget->GetComponentLocation();

			// 위젯이 카메라를 바라보도록 회전값 계산
			FRotator widgetLookRot = UKismetMathLibrary::FindLookAtRotation(widgetLocation, camLocation);

			// 계산된 회전값 설정
			customerWidget->SetWorldRotation(widgetLookRot);
		}
	}

}

void ACustomerAI::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACustomerAI, fsm); 
	DOREPLIFETIME(ACustomerAI, customerWidget);
}

void ACustomerAI::UpdateVisuals(int32 meshIdx)
{
	// 적용할 비주얼 셋 포인터 (복사 방지)
	const FCustomerVisuals* visual = nullptr;

	if(meshIdx == -1)
	{
		visual = &specialVisual;
	}
	else if (regularVisuals.IsValidIndex(mteshIdx))
                              	{
                              		visual = &regularVisuals[meshIdx];
                              	}
                              
                              	// VisualsToApply가 유효하고, 그 안의 메쉬와 AnimBP가 모두 설정되었는지 확인
                              	if (visual && visual->customerMesh && visual->customerAnim)
                              	{
                              		GetMesh()->SetSkeletalMesh(visual->customerMesh); // 1. 메쉬 설정
                              		GetMesh()->SetAnimInsanceClass(visual->customerAnim); // 2. Anim BP 설정
		GetMesh()->SetRelativeScale3D(visual->RelativeScale);   // 3. ★★★ 스케일 설정 추가 ★★★
	}
}

FText ACustomerAI::GetScoreDialogue(EScoreChangeReason reason)
{
	if (!scoreDialogueTable || !fsm) return FText::GetEmpty();

	// reason 값을 FName RowName으로 변환
	const UEnum* reasonEnum = StaticEnum<EScoreChangeReason>();
	const FName rowName = FName(reasonEnum->GetNameStringByValue(static_cast<int64>(reason)));

	// 데이터 테이블에서 해당 행 찾기
	static const FString contextString(TEXT("GetScoreDialogue"));
	FScoreDialogue* dialogueRow = scoreDialogueTable->FindRow<FScoreDialogue>(rowName, contextString);
	if (!dialogueRow) return FText::GetEmpty();
	
	// 손님 성격에 따라 사용할 대사 선택
	const TArray<FText>* variations = nullptr;
	switch (fsm->personality)
	{
	case ECustomerPersonality::Standard:
		{
			variations = &dialogueRow->Standard_Variations;
			break;
		}
	case ECustomerPersonality::Polite:
		{
			variations = &dialogueRow->Polite_Variations;
			break;
		}
	case ECustomerPersonality::Impatient:
		{
			variations = &dialogueRow->Impatient_Variations;
			break;
		}
	case ECustomerPersonality::Rude:
		{
			variations = &dialogueRow->Rude_Variations;
			break;
		}
	default: variations = &dialogueRow->Standard_Variations; break;
	}
	if (!variations || variations->Num() == 0) return FText::GetEmpty();
	
	const int32 RandomIndex = FMath::RandRange(0, variations->Num() - 1);
	return (*variations)[RandomIndex];
}

void ACustomerAI::ShowCustomerDialogue()
{	
	UCustomerUI* customerUI = GetCustomerUIInstance();
	if (fsm && customerUI)
	{
		UpdateCustomerWidget(true, fsm->curDialogue, FLinearColor::White);
	}
}
void ACustomerAI::HideCustomerDialogue()
{
	UCustomerUI* customerUI = GetCustomerUIInstance();
	if (customerUI)
	{
		UpdateCustomerWidget(false);
	}
}

void ACustomerAI::OnRep_FSMStateChanged()
{
	UCustomerUI* customerUI = GetCustomerUIInstance();
	if (!fsm || !customerUI) return;

	if (fsm->curState == EAIState::Ordering)
	{
		// 주문 상태면 주문 대사 표시 요청 (대사는 FSM의 OnRep_Dialogue에서 업데이트될 수 있음)
		UpdateCustomerWidget(true, fsm->curDialogue, FLinearColor::White);
	}
	else
	{
		// 주문 상태가 아니면 무조건 숨김
		UpdateCustomerWidget(false);
	}
}

class UCustomerUI* ACustomerAI::GetCustomerUIInstance()
{
	if (!customerWidget) return nullptr;

	UUserWidget* widgetInst = customerWidget->GetUserWidgetObject();
	if (widgetInst == nullptr && customerWidget->GetWidgetClass())
	{
		customerWidget->InitWidget();
		widgetInst = customerWidget->GetUserWidgetObject();
	}
	return Cast<UCustomerUI>(widgetInst);
}

void ACustomerAI::ShowScoreFeedback(EScoreChangeReason reason)
{
	if (!HasAuthority()) return;
	Multicast_ShowScoreFeedback(reason);
}

// 모든 클라이언트에서 실행
void ACustomerAI::Multicast_ShowScoreFeedback_Implementation(EScoreChangeReason reason)
{
	UCustomerUI* customerUI = GetCustomerUIInstance();
	if (!customerUI) return;

	// 클라이언트 각자 로컬에서 대사를 가져옵니다 (데이터 테이블은 보통 패키징됨).
	// 만약 서버에서만 대사를 결정하고 싶다면, Multicast RPC에 FText와 FLinearColor를 직접 전달해야 합니다.
	// 여기서는 클라이언트에서 대사를 찾는 방식을 사용합니다.
	FText FeedbackDialogue = GetScoreDialogue(reason); // 클라이언트에서 실행되므로 fsm 데이터 필요
	if (!FeedbackDialogue.IsEmpty())
	{
		FLinearColor FeedbackColor = (reason == EScoreChangeReason::CorrectFood) ? FLinearColor::White : FLinearColor::Red;
		UpdateCustomerWidget(true, FeedbackDialogue, FeedbackColor);
	}
}

void ACustomerAI::UpdateCustomerWidget(bool bShow, const FText& textToShow, FLinearColor textColor)
{
	if (!customerWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("UpdateCustomerWidget 실패: customerWidget가 null입니다!"));
		return;
	}

	// 1. 함수가 호출되는지, 텍스트가 비어있는지 확인
	UE_LOG(LogTemp, Warning, TEXT("UpdateCustomerWidget 호출됨. bShow: %s, Text: '%s'"),
		bShow ? TEXT("True") : TEXT("False"), *textToShow.ToString());
	
	// 위젯 표시/숨김 처리
	customerWidget->SetVisibility(bShow);

	// 위젯을 보여줘야하고, 텍스트 내용이 있다면 위젯 업데이트
	if (bShow && !textToShow.IsEmpty())
	{
		// 실제 위젯 인스턴스 가져오고 초기화
		UUserWidget* widgetInst = customerWidget->GetUserWidgetObject();
		if (widgetInst == nullptr && customerWidget->GetWidgetClass())
		{
			UE_LOG(LogTemp, Log, TEXT("위젯 인스턴스가 없어 InitWidget() 호출함."));
			customerWidget->InitWidget();
			widgetInst = customerWidget->GetUserWidgetObject();
		}

		if (widgetInst == nullptr)
		{
			// 2. Widget Class가 할당되지 않았는지 확인
			UE_LOG(LogTemp, Error, TEXT("UpdateCustomerWidget 실패: 위젯 인스턴스가 null입니다. Widget Class가 할당되었나요?"));
			return;
		}
		
		// 위젯 함수 호출하여 텍스트, 색상 설정
		if (UCustomerUI* customerUI = Cast<UCustomerUI>(widgetInst))
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateCustomerWidget 성공: 텍스트를 '%s'로 설정합니다."), *textToShow.ToString());
			customerUI->Event_SetOrderText(textToShow);
			customerUI->Event_SetTextColor(textColor);

			// TODO : 애니메이션 재생 호출
		}
		else if(widgetInst) // Cast 실패 시 로그
		{
			UE_LOG(LogTemp, Error, TEXT("UpdateCustomerWidget 실패: 위젯 인스턴스가 UCustomerUI 타입이 아닙니다! 클래스: %s"), *widgetInst->GetClass()->GetName());
		}
		else // 위젯 인스턴스 자체가 없을 때
		{
			UE_LOG(LogTemp, Error, TEXT("UpdateCustomerWidget 실패: 위젯 인스턴스를 가져오거나 초기화할 수 없습니다!"));
		}
	}
}