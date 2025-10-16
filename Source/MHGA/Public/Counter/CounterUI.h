// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CounterUI.generated.h"

class AReceiptActor;
class UCustomerButtonUI;
class UCanvasPanel;
class UUniformGridPanel;
class UVerticalBox;
class UMenuButtonUI;
class UButton;
class UTextBlock;

USTRUCT(BlueprintType)
struct FOrderArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EBurgerMenu> Menus;
};

UCLASS()
class MHGA_API UCounterUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UCounterUI(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const UEnum* MenuEnumPtr;

	//Order
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* OrderCanvas;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* MenuGrid;
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* SelectedListBox;

	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Order;
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Delete;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMenuButtonUI> MenuButtonClass;

	//Customer
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* CustomerCanvas;
	
	const int32 GridCol = 5;
	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* CustomerGrid;
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* MenuListBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UCustomerButtonUI> CustomerButtonClass;
	
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Ready;

	//Change panel Btn
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Customer;
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Menu;

	//OrderList
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EBurgerMenu> OrderList;
	UPROPERTY(EditAnywhere)
	int32 OrderNum = 100;
	UPROPERTY(EditAnywhere)
	TMap<int32, FOrderArray> OrderMap;

	//CustomerBtn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCustomerButtonUI* CustomerBtn;
	
protected:
	UFUNCTION(BlueprintCallable)
	void OnClickCustomerBtn();
	UFUNCTION(BlueprintCallable)
	void OnClickMenuBtn();
	
public:
	UFUNCTION(BlueprintCallable)
	void AddMenuToList(const EBurgerMenu MenuName);
	UFUNCTION(BlueprintCallable)
	void OrderedMenu(UCustomerButtonUI* Btn);
	
	UFUNCTION(BlueprintCallable)
	void OrderMenuBtn();
	UFUNCTION(BlueprintCallable)
	void DeleteListBtn();
	UFUNCTION(BlueprintCallable)
	void OnMenuReadyBtn();
};
