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

	UFUNCTION()
	void AddIngredient(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void MinusIngredient(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void PrintLog();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// 배열을 맵으로 변환
	TMap<EIngredient, int32> MakeMapFromArray(const TArray<FIngredientStack>& InArray);

	// Data테이블의 레시피 매칭
	// 성공하면 EBurgerMenu BurgerName 반환, 실패하면 EBurgerMenu::WrongBurger
	EBurgerMenu FindMatchingRecipe(UDataTable* DT, const TArray<FIngredientStack>& WrapperIngr);

	
/* Field */
public:
	UPROPERTY(EditAnywhere)
	class UDataTable* BurgerDataTable;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* Collision;
	
	UPROPERTY(EditAnywhere)
	bool bShowLog = true;


protected:


private:
	UPROPERTY()	// 충돌한 재료 저장
	TArray<FIngredientStack> OnAreaIngredients;
	
};
