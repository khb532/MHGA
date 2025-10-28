#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "Potions.generated.h"

UCLASS()
class MHGA_API APortions : public AIngredientBase
{
	GENERATED_BODY()

	/* Method */
public:
	APortions();

	virtual void StartCook() override;
	virtual void ShutdownCook() override;
	
	
private:
	void UpdateMaterial();
	void UpdateIngType(EIngredient ingtype);


	
	/* Field */
public:
	UPROPERTY(EditAnywhere)
	bool b_IsShanghai = false;

	UPROPERTY(EditAnywhere)
	bool b_IsShrimp = false;

	
protected:



private:
	FTimerHandle h_CookTimer;

};
