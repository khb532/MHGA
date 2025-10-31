// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CounterUI.generated.h"

class ACounterPOS;
class AReceiptActor;
class UCustomerButtonUI;
class UCanvasPanel;
class UUniformGridPanel;
class UVerticalBox;
class UMenuButtonUI;
class UButton;
class UTextBlock;

UCLASS()
class MHGA_API UCounterUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UCounterUI(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACounterPOS* PosActor;
	
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


	//CustomerBtn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCustomerButtonUI* CustomerBtn;

	//beep sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* BeepSound;
	
protected:
	UFUNCTION(BlueprintCallable)
	void OnClickCustomerBtn();
	UFUNCTION(BlueprintCallable)
	void OnClickMenuBtn();
	UFUNCTION(BlueprintCallable)
	void OrderMenuBtn();
	UFUNCTION(BlueprintCallable)
	void DeleteListBtn();
	UFUNCTION(BlueprintCallable)
	void OnMenuReadyBtn();
	
public:
	UFUNCTION(BlueprintCallable)
	void SetPosActor(ACounterPOS* Pos);
	UFUNCTION(BlueprintCallable)
	UUniformGridPanel* GetCustomerGrid() {return CustomerGrid;}

	
	UFUNCTION()
	void OnClickCustomerBtnRPC();
	UFUNCTION()
	void OnClickMenuBtnRPC();
	UFUNCTION()
	void OrderMenuBtnRPC();
	UFUNCTION()
	void DeleteListBtnRPC();
	UFUNCTION()
	void OnMenuReadyBtnRPC();
	UFUNCTION()
	void AddMenuToListRPC(const EBurgerMenu MenuName);
	UFUNCTION()
	void CustomerOrderedMenuRPC(UCustomerButtonUI* Btn);

	UPROPERTY()
	class ACustomerManager* manager;
};
