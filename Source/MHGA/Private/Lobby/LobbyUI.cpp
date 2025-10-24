#include "Lobby/LobbyUI.h"

#include "MHGA.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/GameStateBase.h"
#include "Lobby/BoardText.h"
#include "Lobby/LobbyBoard.h"
#include "Player/MHGAPlayerController.h"

void ULobbyUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (LobbyBoard->HasAuthority())
	{
		//BTN_Ready->SetIsEnabled(false);
		TEXT_Ready->SetText(FText::FromString(TEXT("시작")));
	}
	else
	{
		BTN_Ready->SetVisibility(ESlateVisibility::Hidden);
	}

	BTN_Ready->OnClicked.AddDynamic(this, &ULobbyUI::OnClickReady);
	BTN_Run->OnClicked.AddDynamic(this, &ULobbyUI::OnClickRun);
}

void ULobbyUI::Init(ALobbyBoard* InLobbyBoard)
{
	LobbyBoard = InLobbyBoard;
}

void ULobbyUI::OnClickReady()
{
	//pc->ServerRPC_Ready(GS->PlayerArray.IndexOfByKey(PC->PlayerState));
	//준비하면 버튼 못누르게 비활성화
	BTN_Ready->SetIsEnabled(false);

	if (LobbyBoard->HasAuthority())
	{
		PRINTINFO()
		GetWorld()->ServerTravel(TEXT("/Game/Maps/Main?listen"));
	}
}

void ULobbyUI::OnClickRun()
{
	AMHGAPlayerController* pc = Cast<AMHGAPlayerController>(GetWorld()->GetFirstPlayerController());
	pc->ServerRPC_Run();
}

void ULobbyUI::Ready(int32 PlayerNum)
{
	if (LobbyBoard->HasAuthority())
	{
		PRINTINFO()
		GetWorld()->ServerTravel(TEXT("/Game/Maps/Main?listen"));
	}
}

void ULobbyUI::Run()
{
}

void ULobbyUI::Refresh(TArray<FString>& Names)
{
	VB_Player->ClearChildren();
	VB_Ready->ClearChildren();

	for (const FString& Name : Names)
	{
		UBoardText* bt = CreateWidget<UBoardText>(GetWorld(), BoardText);
		UVerticalBoxSlot* vs = VB_Player->AddChildToVerticalBox(bt);
		bt->Init(Name, 95);
		
		UBoardText* ready = CreateWidget<UBoardText>(GetWorld(), BoardText);
		UVerticalBoxSlot* readyslot = VB_Ready->AddChildToVerticalBox(ready);
		ready->Init(FString::Printf(TEXT("교육 중")), 95);
	}
}