// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueData.generated.h"

// 손님 성경
UENUM(BlueprintType)
enum class ECustomerPersonality : uint8
{
	Standard	UMETA(DisplayName = "표준"),
	Polite		UMETA(DisplayName = "공손함"),
	Rude		UMETA(DisplayName = "무례함"),
	Impatient	UMETA(DisplayName = "급함"),
	Special_VIP	UMETA(DisplayName = "매그너스")
};

// 평점 변경 사유
UENUM(BlueprintType)
enum class EScoreChangeReason : uint8
{
	SlowOrder   UMETA(DisplayName="주문 지연"),
	SlowCook    UMETA(DisplayName="조리 지연"), 
	WrongFood   UMETA(DisplayName="잘못된 음식"),
	CorrectFood UMETA(DisplayName="올바른 음식")
};

// 주문 대사 구조체
USTRUCT(BlueprintType)
struct FOrderDialogue : public FTableRowBase
{
	GENERATED_BODY()

	// 사용될 메뉴의 표시 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText MenuDisplayName;

	// "표준" 성격의 대사 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TArray<FText> Standard_Variations;

	// "공손함" 성격의 대사 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TArray<FText> Polite_Variations;

	// "무례한" 성격의 대사 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TArray<FText> Rude_Variations;
    
	// "급한" 성격의 대사 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TArray<FText> Impatient_Variations;
	
};

// 평판 대사 데이터 구조체
USTRUCT(BlueprintType)
struct FScoreDialogue : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> Standard_Variations;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> Polite_Variations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> Rude_Variations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> Impatient_Variations;
};