// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class MHGA_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* finalScore;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* orderScoreBox;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* cookScoreBox;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* foodScoreBox;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* startText;
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* startCanvas;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* StartAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FinalScoreAnim;

	UFUNCTION()
	void PlayStartAnim();
	UFUNCTION()
	void PlayFinalScoreAnim();
};
