// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "OrderDialogue.generated.h"

UENUM(BlueprintType)
enum class ECustomerPersonality : uint8
{
	Standard	UMETA(DisplayName = "표준"),
	Polite		UMETA(DisplayName = "공손함"),
	Rude		UMETA(DisplayName = "무례함"),
	Impatient	UMETA(DisplayName = "급함")
};

// 데이터 테이블 행 구조체
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

UCLASS()
class MHGA_API UMyDataTable : public UDataTable
{
	GENERATED_BODY()
	
};
