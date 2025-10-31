#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "SauceBottle.generated.h"

class USoundBase;

UCLASS()
class MHGA_API ASauceBottle : public AIngredientBase
{
	GENERATED_BODY()

/* Method */
public:
	ASauceBottle();

	virtual void Tick(float DeltaTime) override;

	void ShootSauce();

	
protected:
	virtual void BeginPlay() override;
	virtual void OnGrabbed(AMHGACharacter* Player) override;
	virtual void OnPut() override;
	virtual void OnUse() override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_PlaySauceSound();


/* Field */
public:
	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* Arrow;

	UPROPERTY(EditAnywhere)
	bool bShowLog = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AIngredientBase> SauceClass;
	
protected:

private:
	bool bGrabbed = false;

	UPROPERTY()
	USoundBase* SauceSound;
};
