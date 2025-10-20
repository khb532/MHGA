// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomerAI.h"

#include "AIController.h"
#include "public/AI/CustomerUI.h"
#include "AI/CustomerFSM.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "DSP/AudioDebuggingUtilities.h"

// Sets default values
ACustomerAI::ACustomerAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	fsm = CreateDefaultSubobject<UCustomerFSM>(TEXT("FSM"));
	
	judgeWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("JudgeWidget"));
	judgeWidget->SetupAttachment(GetRootComponent());
	// 항상 화면을 향하게
	judgeWidget->SetWidgetSpace(EWidgetSpace::Screen);
	judgeWidget->SetDrawSize(FVector2D(150, 50));
	judgeWidget->SetVisibility(false);
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

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

}

FText ACustomerAI::GetOrderTextFromFSM()
{
	if (fsm)
	{
		return fsm->GetOrderedMenuAsText();
	}

	return NSLOCTEXT("BurgerMenu", "Shrimp", "없어요");
}

void ACustomerAI::ShowOrderUI()
{
	// OrderWidgetClass가 에디터에서 지정되었는지 확인
	if (!orderWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("OrderWidgetClass is not set in %s"), *GetName());
		return;
	}

	// 위젯이 아직 생성되지 않았다면 새로 생성
	if (!orderWidgetInst)
	{
		orderWidgetInst = CreateWidget(GetWorld(), orderWidget);
	}
    
	// 위젯이 유효한지 다시 한번 확인하고, OwnerAI를 설정한 뒤 화면에 추가
	if (orderWidgetInst)
	{
		// 리페어런팅을 했다면, 자식 클래스로 형 변환하여 OwnerAI 변수에 접근
		if (UCustomerUI* OrderBubble = Cast<UCustomerUI>(orderWidgetInst))
		{
			OrderBubble->ownerAI = this; // 'this'는 이 코드를 실행하는 인스턴스 자신
		}

		orderWidgetInst->AddToViewport();
	}
}

void ACustomerAI::HideOrderUI()
{
	if (orderWidgetInst && orderWidgetInst->IsInViewport())
	{
		orderWidgetInst->RemoveFromParent();
	}
}

void ACustomerAI::ShowReputationText(bool bIsPositive)
{
	if (!judgeWidget) return;

	// 위젯 컴포넌트를 보이게 합니다.
	judgeWidget->SetVisibility(true);

	// 컴포넌트가 가지고 있는 실제 위젯 인스턴스를 가져옵니다.
	UUserWidget* widgetInst = judgeWidget->GetUserWidgetObject();
	if (widgetInst)
	{
		// 위젯 블루프린트에 만들어 둔 'ShowFeedbackText' 이벤트를 이름으로 찾아 호출합니다.
		UFunction* function = widgetInst->FindFunction(FName("ShowFeedbackText"));
		if (function)
		{
			// 함수에 전달할 파라미터를 준비합니다.
			struct FFeedbackParams
			{
				FText Message;
				FLinearColor Color;
			};

			FFeedbackParams Params;
			if (bIsPositive)
			{
				Params.Message = NSLOCTEXT("Feedback", "Good", ":)");
				Params.Color = FLinearColor::White;
			}
			else
			{
				Params.Message = NSLOCTEXT("Feedback", "Bad", ":(");
				Params.Color = FLinearColor::Red;
			}

			// 함수를 실행합니다.
			widgetInst->ProcessEvent(function, &Params);
		}
	}
}
