#include "Lobby/LobbyUI.h"

void ULobbyUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULobbyUI::Init(ALobbyBoard* InLobbyBoard)
{
	LobbyBoard = InLobbyBoard;
}
