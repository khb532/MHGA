// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JobButtonUI.generated.h"

class UTextBlock;
class UImage;
class UButton;
/**
 * 
 */
UCLASS()
class MHGA_API UJobButtonUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	int32 SessionIdx = 0;
	
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_Session;

	UPROPERTY(meta=(BindWidget))
	UImage* IMG_Main;
	UPROPERTY(meta=(BindWidget))
	UImage* IMG_Sub;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TEXT_Name;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TEXT_Description;

private:
	UFUNCTION() void OnClickSessionBtn();
	
public:
	void SetImage();
	void Init(FString Name, int32 Current, int32 Max, int32 Idx);
};
