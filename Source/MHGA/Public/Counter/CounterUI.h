// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CounterUI.generated.h"

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

protected:
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

public:
	UFUNCTION(BlueprintCallable)
	void AddMenuToList(const FString& MenuName);
	UFUNCTION(BlueprintCallable)
	void OrderMenu();
	UFUNCTION(BlueprintCallable)
	void DeleteList();
};
