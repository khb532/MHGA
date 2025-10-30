// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectUI.generated.h"

class ALobbyPlayerState;
class UButton;
/**
 * 
 */
UCLASS()
class MHGA_API UCharacterSelectUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	ALobbyPlayerState* PS;

	UPROPERTY(meta=(BindWidget))
	UButton* BTN_P1;
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_P2;
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_P3;
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_P4;

protected:
	UFUNCTION() void OnClicked_P1();
	UFUNCTION() void OnClicked_P2();
	UFUNCTION() void OnClicked_P3();
	UFUNCTION() void OnClicked_P4();

public:
	void SetPS(ALobbyPlayerState* inPS) {PS = inPS;}
};
