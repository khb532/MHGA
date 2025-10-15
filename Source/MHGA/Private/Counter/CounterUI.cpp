#include "Counter/CounterUI.h"
#include "BurgerData.h"
#include "MHGA.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/VerticalBox.h"
#include "Counter/MenuButtonUI.h"
#include "Counter/CustomerButtonUI.h"
#include "Counter/ReceiptActor.h"


UCounterUI::UCounterUI(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UMenuButtonUI> mb(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_MenuBtn.WBP_MenuBtn_C'"));
	if (mb.Succeeded())
		MenuButtonClass = mb.Class;

	ConstructorHelpers::FClassFinder<UCustomerButtonUI> cb(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_CustomerBtn.WBP_CustomerBtn_C'"));
	if (cb.Succeeded())
		CustomerButtonClass = cb.Class;
}

void UCounterUI::NativeConstruct()
{
	Super::NativeConstruct();

	BTN_Order->OnClicked.AddDynamic(this, &UCounterUI::OrderMenuBtn);
	BTN_Delete->OnClicked.AddDynamic(this, &UCounterUI::DeleteListBtn);
	BTN_Customer->OnClicked.AddDynamic(this, &UCounterUI::OnClickCustomerBtn);
	BTN_Menu->OnClicked.AddDynamic(this, &UCounterUI::OnClickMenuBtn);
	BTN_Ready->OnClicked.AddDynamic(this, &UCounterUI::OnMenuReadyBtn);

	MenuEnumPtr = StaticEnum<EBurgerMenu>();
	const int32 NumEnums = MenuEnumPtr->NumEnums() - 1;
	const int32 NumCols = 3;
	int32 ValidIndex = 0;

	for (int32 i = 0; i < NumEnums; ++i)
	{
		EBurgerMenu MenuValue = static_cast<EBurgerMenu>(i);

		if (MenuValue == EBurgerMenu::None || MenuValue == EBurgerMenu::WrongBurger)
			continue;
		
		UMenuButtonUI* NewButton = CreateWidget<UMenuButtonUI>(GetWorld(), MenuButtonClass);
		if (NewButton)
		{
			NewButton->Init(MenuValue, this);

			int32 Row = ValidIndex / NumCols;
			int32 Col = ValidIndex % NumCols;
			ValidIndex++;

			MenuGrid->AddChildToUniformGrid(NewButton, Row, Col);
		}
	}

	CustomerCanvas->SetVisibility(ESlateVisibility::Hidden);
}

void UCounterUI::OnClickCustomerBtn()
{
	CustomerCanvas->SetVisibility(ESlateVisibility::Visible);
	OrderCanvas->SetVisibility(ESlateVisibility::Hidden);
}

void UCounterUI::OnClickMenuBtn()
{
	CustomerCanvas->SetVisibility(ESlateVisibility::Hidden);
	OrderCanvas->SetVisibility(ESlateVisibility::Visible);
}

void UCounterUI::AddMenuToList(const EBurgerMenu MenuName)
{
	UTextBlock* NewText = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
	if (NewText)
	{
		OrderList.Add(MenuName);
		NewText->SetText(MenuEnumPtr->GetDisplayNameTextByValue(static_cast<int64>(MenuName)));
		SelectedListBox->AddChildToVerticalBox(NewText);
	}
}

void UCounterUI::OrderedMenu(UCustomerButtonUI* Btn)
{
	MenuListBox->ClearChildren();
	
	CustomerBtn = Btn;
	TArray<EBurgerMenu> Menu = CustomerBtn->GetMenuInfo();
	for (EBurgerMenu M : Menu)
	{
		UTextBlock* NewText = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
		if (NewText)
		{
			NewText->SetText(MenuEnumPtr->GetDisplayNameTextByValue(static_cast<int64>(M)));
			MenuListBox->AddChildToVerticalBox(NewText);
		}
	}
}

void UCounterUI::OrderMenuBtn()
{
	if (OrderList.Num() < 1) return;
	
	OrderMap.FindOrAdd(OrderNum) = {OrderList};
	//PRINTLOG(TEXT("%d, %d"), OrderNum, OrderMap[OrderNum].Menus.Num());

	//주문완료 시 해당 주문 번호 버튼 생성
	UCustomerButtonUI* NewCustomerBtn = CreateWidget<UCustomerButtonUI>(GetWorld(), CustomerButtonClass);
	if (NewCustomerBtn)
	{
		NewCustomerBtn->Init(OrderList, OrderNum, this);
		int32 NumChildren = CustomerGrid->GetChildrenCount();
		int32 Row = NumChildren / GridCol;
		int32 Col = NumChildren % GridCol;

		CustomerGrid->AddChildToUniformGrid(NewCustomerBtn, Row, Col);
	}
	
	//영수증 출력
	AReceiptActor* NewReceipt = GetWorld()->SpawnActor<AReceiptActor>(AReceiptActor::StaticClass());
	if (NewReceipt)
	{
		TArray<FString> MenuStrings;
		for (EBurgerMenu Menu : OrderList)
		{
			FString MenuName = MenuEnumPtr->GetDisplayNameTextByValue(static_cast<int64>(Menu)).ToString();
			MenuStrings.Add(MenuName);
		}
		NewReceipt->Init(OrderNum, MenuStrings);
	}
	
	//TODO : AI가 주문을 마친 후 로직 추가

	OrderNum++;
	DeleteListBtn();
}

void UCounterUI::DeleteListBtn()
{
	OrderList.Empty();
	SelectedListBox->ClearChildren();
}

void UCounterUI::OnMenuReadyBtn()
{
	if (CustomerBtn == nullptr)
		return;
	
	OrderMap.Remove(CustomerBtn->GetNum());
	MenuListBox->ClearChildren();
	CustomerGrid->RemoveChild(CustomerBtn);

	//그리드 정렬
	TArray<UWidget*> Widgets = CustomerGrid->GetAllChildren();
	for (int32 i = 0; i < Widgets.Num(); ++i)
	{
		if (UUniformGridSlot* S = Cast<UUniformGridSlot>(Widgets[i]->Slot))
		{
			const int32 Row = i / GridCol;
			const int32 Col = i % GridCol;
			S->SetRow(Row);
			S->SetColumn(Col);
		}
	}
	//TODO : 손님 호출
	
	CustomerBtn = nullptr;
}
