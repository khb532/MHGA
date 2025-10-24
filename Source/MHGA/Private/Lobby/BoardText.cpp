
#include "Lobby/BoardText.h"

#include "Components/SizeBox.h"
#include "Components/TextBlock.h"


void UBoardText::Init(FString txt, float sizeY)
{
	SizeBox->SetHeightOverride(sizeY);
	TEXT_Content->SetText(FText::FromString(*txt));
}
