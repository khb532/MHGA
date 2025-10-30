#include "Lobby/CharacterSelectUI.h"

#include "Components/Button.h"
#include "Lobby/LobbyPlayerState.h"

void UCharacterSelectUI::NativeConstruct()
{
	Super::NativeConstruct();

	BTN_P1->OnClicked.AddDynamic(this, &UCharacterSelectUI::OnClicked_P1);
	BTN_P2->OnClicked.AddDynamic(this, &UCharacterSelectUI::OnClicked_P2);
	BTN_P3->OnClicked.AddDynamic(this, &UCharacterSelectUI::OnClicked_P3);
	BTN_P4->OnClicked.AddDynamic(this, &UCharacterSelectUI::OnClicked_P4);
}

void UCharacterSelectUI::OnClicked_P1()
{
	PS->ServerRPC_SelectPlayer(0);
}

void UCharacterSelectUI::OnClicked_P2()
{
	PS->ServerRPC_SelectPlayer(1);
}

void UCharacterSelectUI::OnClicked_P3()
{
	PS->ServerRPC_SelectPlayer(2);
}

void UCharacterSelectUI::OnClicked_P4()
{
	PS->ServerRPC_SelectPlayer(3);
}
