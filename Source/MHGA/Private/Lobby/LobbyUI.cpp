#include "Lobby/LobbyUI.h"

#include "MHGA.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"
#include "Interfaces/OnlineSessionInterface.h"
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

	BTN_Ready->OnClicked.AddDynamic(this, &ULobbyUI::OnClickReady);
	BTN_Run->OnClicked.AddDynamic(this, &ULobbyUI::OnClickRun);

	IsPlayerReady.Init(false, 4);
	for (int i = 0; i<VB_Player->GetChildrenCount(); i++)
	{
		PlayerArr.Add(Cast<UTextBlock>(VB_Player->GetChildAt(i)));
		ReadyArr.Add(Cast<UTextBlock>(VB_Ready->GetChildAt(i)));
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	AGameStateBase* GS = GetWorld()->GetGameState();
	PlayerCount = GS->PlayerArray.IndexOfByKey(PC->PlayerState);
	Refresh(PlayerCount);
}

void ULobbyUI::Init(ALobbyBoard* InLobbyBoard)
{
	LobbyBoard = InLobbyBoard;
}

void ULobbyUI::OnClickReady()
{
	AMHGAPlayerController* pc = Cast<AMHGAPlayerController>(GetWorld()->GetFirstPlayerController());
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	AGameStateBase* GS = GetWorld()->GetGameState();
	pc->ServerRPC_Ready(GS->PlayerArray.IndexOfByKey(PC->PlayerState));

	//준비하면 버튼 못누르게 비활성화
	BTN_Ready->SetIsEnabled(false);
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
		IsPlayerReady[0] = true;
		for (int32 i = 0; i<PlayerCount; i++)
		{
			if (IsPlayerReady[i] == false)
				return;
		}

		PRINTINFO()
		//TODO : 요기에 시작
	}
	else
	{
		ReadyArr[PlayerNum]->SetText(FText::FromString(TEXT("준비 완료")));
		IsPlayerReady[PlayerNum] = true;
	}
}

void ULobbyUI::Run()
{
}

void ULobbyUI::Refresh(int32 PlayerNum)
{
	PlayerCount = PlayerNum;
	for (int i = 0; i<=PlayerNum; i++)
	{
		PlayerArr[i]->SetVisibility(ESlateVisibility::Visible);
		ReadyArr[i]->SetVisibility(ESlateVisibility::Visible);
	}
}
