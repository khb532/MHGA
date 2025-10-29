// Fill out your copyright notice in the Description page of Project Settings.


#include "StartUI/StartUI.h"

#include "MHGAGameInstance.h"
#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/UniformGridPanel.h"
#include "StartUI/JobButtonUI.h"

UStartUI::UStartUI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UJobButtonUI> job(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Start/WBP_JobBtn.WBP_JobBtn_C'"));
	if (job.Succeeded())
		JobBtn = job.Class;
}

void UStartUI::NativeConstruct()
{
	Super::NativeConstruct();

	GI = Cast<UMHGAGameInstance>(GetGameInstance());
	GI->FindCompleteDelegate.BindUObject(this, &UStartUI::OnFindComplete);

	Btn_Login->OnClicked.AddDynamic(this, &UStartUI::OnClickLoginBtn);
	Btn_SearchJob->OnClicked.AddDynamic(this, &UStartUI::OnClickSearchBtn);
	Btn_Refresh->OnClicked.AddDynamic(this, &UStartUI::OnClickRefreshBtn);
	Btn_MakeJob->OnClicked.AddDynamic(this, &UStartUI::OnClickMakeJobBtn);
	Btn_MakeJobExit->OnClicked.AddDynamic(this, &UStartUI::OnClickMakeJobExitBtn);
	Btn_RegisterJob->OnClicked.AddDynamic(this, &UStartUI::OnClickRegisterJobBtn);

	Input_Number->OnTextChanged.AddDynamic(this, &UStartUI::OnInputNumChange);

	Canvas_Start->SetVisibility(ESlateVisibility::Hidden);
	Canvas_Session->SetVisibility(ESlateVisibility::Hidden);
	Canvas_MakeJob->SetVisibility(ESlateVisibility::Hidden);
}

void UStartUI::OnClickLoginBtn()
{
	if (Input_Login->GetText().ToString().Len() > 0)
	{
		GI->SetPlayerName(Input_Login->GetText().ToString());
	}

	Canvas_Login->SetVisibility(ESlateVisibility::Hidden);
	Canvas_Start->SetVisibility(ESlateVisibility::Visible);
}

void UStartUI::OnClickSearchBtn()
{
	Canvas_Session->SetVisibility(ESlateVisibility::Visible);
	Canvas_Start->SetVisibility(ESlateVisibility::Hidden);

	OnClickRefreshBtn();
}

void UStartUI::OnClickRefreshBtn()
{
	Grid_Panel->ClearChildren();
	Btn_Refresh->SetIsEnabled(false);
	
	GI->FindOtherSession();
}

void UStartUI::OnClickMakeJobBtn()
{
	Canvas_MakeJob->SetVisibility(ESlateVisibility::Visible);
}

void UStartUI::OnClickMakeJobExitBtn()
{
	Canvas_MakeJob->SetVisibility(ESlateVisibility::Hidden);
	Input_Name->SetText(FText::GetEmpty());
	Input_Number->SetText(FText::GetEmpty());
}

void UStartUI::OnClickRegisterJobBtn()
{
	FString AlbaName = Input_Name->GetText().ToString();
	int32 Number = FCString::Atoi(*Input_Number->GetText().ToString());

	//OnClickMakeJobExitBtn();
	GI->CreateMySession(AlbaName, Number);
}

void UStartUI::OnInputNumChange(const FText& Text)
{
	FString Str = Text.ToString();
	FString Filtered;
	for (TCHAR C : Str)
	{
		if (FChar::IsDigit(C))
			Filtered.AppendChar(C);
	}

	if (Filtered.Len() > 1)
		Filtered = Filtered.Left(1);

	if (Filtered.IsEmpty())
	{
		Input_Number->SetText(FText::FromString(TEXT("")));
		return;
	}
	
	int32 Num = FMath::Clamp(FCString::Atoi(*Filtered), 2, 4);
	FString ClampedStr = FString::FromInt(Num);

	Input_Number->SetText(FText::FromString(ClampedStr));
}

void UStartUI::OnFindComplete(TArray<FOnlineSessionSearchResult>& Results)
{
	for (int i = 0; i<Results.Num(); i++)
	{
		int32 MaxPlayers = Results[i].Session.SessionSettings.NumPublicConnections; // 최대 플레이어 수
		int32 CurrentPlayers; // 현재 플레이어 수
		Results[i].Session.SessionSettings.Get(FName("PlayerCount"), CurrentPlayers);
		FString DisplayName; //세션 이름
		Results[i].Session.SessionSettings.Get(FName("NAME"), DisplayName);
		DisplayName = GI->StringBase64Decode(DisplayName);
		UE_LOG(LogTemp, Warning, TEXT("세션: %i, 이름: %s, 인원: %d/%d"), i, *DisplayName, CurrentPlayers, MaxPlayers);

		UJobButtonUI* Job = CreateWidget<UJobButtonUI>(GetWorld(), JobBtn);
		Job->Init(DisplayName, CurrentPlayers, MaxPlayers, i);
		Grid_Panel->AddChildToUniformGrid(Job, i/ColumnCount, i%ColumnCount);
	}

	Btn_Refresh->SetIsEnabled(true);
}
