#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "Portions.generated.h"

UENUM(BlueprintType)
enum class EPortionCookState : uint8
{
	Raw,
	Cooked
};

UCLASS()
class MHGA_API APortions : public AIngredientBase
{
	GENERATED_BODY()

	/* Method */
public:
	APortions();

	virtual void StartCook() override;
	virtual void ShutdownCook() override;

	UFUNCTION()
	void OnRep_CookState();

	void OnCookingComplete();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void UpdateMaterial();


	
	/* Field */
public:
	UPROPERTY(EditAnywhere)
	bool b_IsShanghai = false;

	UPROPERTY(EditAnywhere)
	bool b_IsShrimp = false;

	UPROPERTY(ReplicatedUsing=OnRep_CookState)
	EPortionCookState CookState;

private:
	FTimerHandle h_CookTimer;

};
