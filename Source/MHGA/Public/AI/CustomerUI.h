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
};
