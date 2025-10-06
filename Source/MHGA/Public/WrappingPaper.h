#pragma once

#include "CoreMinimal.h"
#include "BurgerData.h"
#include "GameFramework/Actor.h"
#include "WrappingPaper.generated.h"

UCLASS()
class MHGA_API AWrappingPaper : public AActor
{
	GENERATED_BODY()

/* Method */
public:
	AWrappingPaper();
	virtual void Tick(float DeltaTime) override;

	
protected:
	virtual void BeginPlay() override;

private:
	EBurgerMenu CheckIngredients();

	// 배열을 맵으로 변환
	TMap<EIngredient, int32> MakeMapFromArray(const TArray<FIngredientStack>& InArray);

	// Data테이블의 레시피 매칭
	// 성공하면 EBurgerMenu BurgerName 반환, 실패하면 EBurgerMenu::WrongBurger
	EBurgerMenu FindMatchingRecipe(UDataTable* DT, const TArray<FIngredientStack>& WrapperIngr);

	
/* Field */
public:
	UPROPERTY(EditAnywhere)
	class UDataTable* BurgerDataTable;


protected:


private:
	
	UPROPERTY()
	class UBoxComponent* Collision;

	UPROPERTY()
	TArray<FIngredientStack> OnAreaIngredients;
	
};
