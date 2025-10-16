#include "Counter/CustomerButtonUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Counter/CounterUI.h"
#include "Player/MHGAPlayerController.h"

void UCustomerButtonUI::NativeConstruct()
{
	Super::NativeConstruct();

	BTN_Customer->OnClicked.AddDynamic(this, &UCustomerButtonUI::OnClickedBtn);
}

void UCustomerButtonUI::OnClickedBtn()
{
	AMHGAPlayerController* pc = Cast<AMHGAPlayerController>(GetWorld()->GetFirstPlayerController());
	pc->ServerRPC_CustomerOrderedMenu(Num);
}

void UCustomerButtonUI::Init(TArray<EBurgerMenu> InMenu, int32 InNum, UCounterUI* InOwner)
{
	Num = InNum;
	Menus = InMenu;
	TEXT_CustomerNum->SetText(FText::AsNumber(InNum));
	CounterUI = InOwner;
}

int32 UCustomerButtonUI::GetNum()
{
	return Num;
}

TArray<EBurgerMenu>& UCustomerButtonUI::GetMenuInfo()
{
	return Menus;
}
