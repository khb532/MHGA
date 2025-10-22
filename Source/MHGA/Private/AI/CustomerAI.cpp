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
	customerWidget->SetPivot(FVector2D(0.5f, 1.0f));
	
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
	if (!fsm || !customerWidget) return;

	// 위젯 컴포넌트를 보이게 설정 (서버에서 호출시 클라이언트에 복제됨)
	customerWidget->SetVisibility(true);

	// 보유중인 실제 위젯 가져오기
	UUserWidget* widgetInst = customerWidget->GetUserWidgetObject();

	// 위젯 초기화가 안되어있으면 실행
	if (widgetInst == nullptr && customerWidget->GetWidgetClass())
	{
		customerWidget->InitWidget();
		widgetInst = customerWidget->GetUserWidgetObject();
	}

	if (UCustomerUI* orderWidget = Cast<UCustomerUI>(widgetInst))
	{
		orderWidget->Event_SetOrderText(fsm->GetCurrentDialogue());
	}
}

void ACustomerAI::HideOrderUI()
{
	if (customerWidget)
	{
		customerWidget->SetVisibility(false);
	}
}

void ACustomerAI::ShowReputationText(bool bIsPositive)
{
	if (HasAuthority())
	{
		Multicast_ShowReputationText(bIsPositive);
	}
}

void ACustomerAI::Multicast_ShowReputationText_Implementation(bool bIsPositive)
{
	if (!customerWidget) return;

	// 텍스트, 색상 변수
	FText FeedbackText;
	FLinearColor FeedbackColor;

	if (bIsPositive)
	{
		FeedbackText = NSLOCTEXT("Feedback", "Good", ":)");
		FeedbackColor = FLinearColor::White;
	}
	else
	{
		FeedbackText = NSLOCTEXT("Feedback", "Bad", ":(");
		FeedbackColor = FLinearColor::Red;
	}
	// 컴포넌트가 가지고 있는 실제 위젯 가져오기
	UUserWidget* widgetInst = customerWidget->GetUserWidgetObject();
	// 초기화 확인 로직
	if (widgetInst == nullptr && customerWidget->GetWidgetClass())
	{
		customerWidget->InitWidget();
		widgetInst = customerWidget->GetUserWidgetObject();
	}

	// 이벤트 호출
	if (UCustomerUI* reputationUI = Cast<UCustomerUI>(widgetInst))
	{
		reputationUI->Event_SetOrderText(FeedbackText);
		reputationUI->Event_SetTextColor(FeedbackColor);
	}

	// 위젯 컴포넌트 보이게 설정
	customerWidget->SetVisibility(true);
}
