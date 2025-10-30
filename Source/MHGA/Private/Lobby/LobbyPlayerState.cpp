// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyPlayerState.h"

#include "MHGAGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Lobby/CharacterSelectUI.h"
#include "Player/MHGACharacter.h"

ALobbyPlayerState::ALobbyPlayerState()
{
	ConstructorHelpers::FClassFinder<UCharacterSelectUI> ui(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Lobby/WBP_CharacterSelectUI.WBP_CharacterSelectUI_C'"));
	if (ui.Succeeded())
		SelectUIClass = ui.Class;
}

void ALobbyPlayerState::ClientRPC_MakeReadyUI_Implementation()
{
	UCharacterSelectUI* SelectUI = CreateWidget<UCharacterSelectUI>(GetWorld(), SelectUIClass);
	SelectUI->SetPS(this);
	SelectUI->AddToViewport();
}

void ALobbyPlayerState::ServerRPC_SelectPlayer_Implementation(int32 meshIdx)
{
	//해당 플레이어가 어떤 플레이어를 선택했는지 가지고 오기
	UMHGAGameInstance* gi = GetGameInstance<UMHGAGameInstance>();
	gi->SetSelectCharacter(GetPlayerName(), meshIdx);
	
	AMHGACharacter* player = Cast<AMHGACharacter>(GetPlayerController()->GetPawn());
	player->SetFirstPersonMesh(gi->GetSkeletalMesh(meshIdx));
}
