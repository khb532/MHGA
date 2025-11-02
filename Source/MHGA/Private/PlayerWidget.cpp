// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"

void UPlayerWidget::PlayStartAnim()
{
	if (StartAnim)
	{
		PlayAnimation(StartAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
}

void UPlayerWidget::PlayFinalScoreAnim()
{
	if (FinalScoreAnim)
	{
		PlayAnimation(FinalScoreAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
}
