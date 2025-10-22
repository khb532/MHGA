// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/CustomerAI.h"
#include "Blueprint/UserWidget.h"
#include "CustomerUI.generated.h"

/**
 * 
 */

UCLASS()
class MHGA_API UCustomerUI : public UUserWidget
{
	GENERATED_BODY()

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ACustomerAI* ownerAI;

	// 텍스트 설정
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void Event_SetOrderText(const FText& Dialogue);

	// 텍스트 색상 설정
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void Event_SetTextColor(FLinearColor TextColor);
};
