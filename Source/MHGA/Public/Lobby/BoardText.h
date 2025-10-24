// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoardText.generated.h"

class USizeBox;
class UTextBlock;
UCLASS()
class MHGA_API UBoardText : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	USizeBox* SizeBox;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TEXT_Content;

public:
	void Init(FString txt, float sizeY);
};
