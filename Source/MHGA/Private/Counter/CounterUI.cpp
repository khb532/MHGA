#include "Counter/CounterUI.h"
#include "BurgerData.h"
#include "MHGA.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "Counter/MenuButtonUI.h"


UCounterUI::UCounterUI(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UMenuButtonUI> mb(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_MenuBtn.WBP_MenuBtn_C'"));
	if (mb.Succeeded())
		MenuButtonClass = mb.Class;
}

void UCounterUI::NativeConstruct()
{
	Super::NativeConstruct();

	BTN_Order->OnClicked.AddDynamic(this, &UCounterUI::OrderMenu);
	BTN_Delete->OnClicked.AddDynamic(this, &UCounterUI::DeleteList);

	const UEnum* EnumPtr = StaticEnum<EBurgerMenu>();
	const int32 NumEnums = EnumPtr->NumEnums() - 1;
	const int32 NumCols = 3;
	int32 ValidIndex = 0;

	for (int32 i = 0; i < NumEnums; ++i)
	{
		EBurgerMenu MenuValue = static_cast<EBurgerMenu>(i);

		if (MenuValue == EBurgerMenu::None || MenuValue == EBurgerMenu::WrongBurger)
			continue;

		FString NameString = EnumPtr->GetDisplayNameTextByIndex(i).ToString();

		UMenuButtonUI* NewButton = CreateWidget<UMenuButtonUI>(GetWorld(), MenuButtonClass);
		if (NewButton)
		{
			NewButton->Init(NameString, this);

			int32 Row = ValidIndex / NumCols;
			int32 Col = ValidIndex % NumCols;
			ValidIndex++;

			MenuGrid->AddChildToUniformGrid(NewButton, Row, Col);
		}
	}
}

void UCounterUI::AddMenuToList(const FString& MenuName)
{
	UTextBlock* NewText = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
	if (NewText)
	{
		NewText->SetText(FText::FromString(MenuName));
		SelectedListBox->AddChildToVerticalBox(NewText);
	}
}

void UCounterUI::OrderMenu()
{
	PRINTLOG(TEXT("ORDER"));

	DeleteList();
}

void UCounterUI::DeleteList()
{
	SelectedListBox->ClearChildren();
}
