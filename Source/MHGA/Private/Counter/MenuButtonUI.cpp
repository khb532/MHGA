#include "Counter/MenuButtonUI.h"

#include "MHGA.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Counter/CounterUI.h"
#include "Player/MHGAPlayerController.h"

void UMenuButtonUI::NativeConstruct()
{
	Super::NativeConstruct();

	BTN_Menu->OnClicked.AddDynamic(this, &UMenuButtonUI::OnClickedMenu);
}

void UMenuButtonUI::OnClickedMenu()
{
	AMHGAPlayerController* pc = Cast<AMHGAPlayerController>(GetWorld()->GetFirstPlayerController());
	pc->ServerRPC_AddMenuToList(MenuName);
	
	PRINTLOG(TEXT("OnClickedMenu"));
}

void UMenuButtonUI::Init(const EBurgerMenu InName, UCounterUI* InOwner)
{
	MenuName = InName;
	CounterUI = InOwner;
	TEXT_MenuName->SetText(StaticEnum<EBurgerMenu>()->GetDisplayNameTextByValue(static_cast<int64>(MenuName)));
}