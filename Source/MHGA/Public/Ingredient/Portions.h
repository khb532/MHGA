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

	UFUNCTION(Server, Reliable)
	void ServerRPC_StartCook();

	UFUNCTION(Server, Reliable)
	void ServerRPC_ShutdownCook();

	UFUNCTION()
	void OnRep_CookState();

	void OnCookingComplete();

	UFUNCTION(NetMulticast,Reliable)
	void MulticastRPC_PlayFrySfx();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_StopFrySfx();
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;

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

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> m_Material;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UAudioComponent> m_sfx_FrySoundComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundBase> m_sfx_FrySound;

	

private:
	FTimerHandle h_CookTimer;

};
