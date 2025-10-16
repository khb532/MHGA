// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CustomerAI.h"

#include "AIController.h"
#include "CustomerUI.h"
#include "AI/CustomerFSM.h"
#include "Blueprint/UserWidget.h"
#include "DSP/AudioDebuggingUtilities.h"

// Sets default values
ACustomerAI::ACustomerAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	fsm = CreateDefaultSubobject<UCustomerFSM>(TEXT("FSM"));

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
	// 1. OrderWidgetClass가 에디터에서 지정되었는지 확인합니다.
	if (!orderWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("OrderWidgetClass is not set in %s"), *GetName());
		return;
	}

	// 2. 위젯이 아직 생성되지 않았다면 새로 생성합니다.
	if (!orderWidgetInst)
	{
		orderWidgetInst = CreateWidget(GetWorld(), orderWidget);
	}
    
	// 3. 위젯이 유효한지 다시 한번 확인하고, OwnerAI를 설정한 뒤 화면에 추가합니다.
	if (orderWidgetInst)
	{
		// 2단계에서 리페어런팅을 했다면, 자식 클래스로 형 변환하여 OwnerAI 변수에 접근할 수 있습니다.
		if (UCustomerUI* OrderBubble = Cast<UCustomerUI>(orderWidgetInst))
		{
			OrderBubble->ownerAI = this; // 'this'는 이 코드를 실행하는 AYourAIActor 인스턴스 자신입니다.
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
