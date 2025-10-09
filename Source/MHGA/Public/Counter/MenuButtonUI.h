// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuButtonUI.generated.h"

class UCounterUI;
class UTextBlock;
class UButton;

UCLASS()
class MHGA_API UMenuButtonUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UCounterUI* CounterUI;
	UPROPERTY()
	FString MenuName;
	
protected:
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Menu;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TEXT_MenuName;

protected:
	UFUNCTION(BlueprintCallable)
	void OnClickedMenu();
	
public:
	void Init(const FString& InName, UCounterUI* InOwner);
};
