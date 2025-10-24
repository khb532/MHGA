#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "Patty.generated.h"

// 패티의 요리 상태
UENUM(BlueprintType)
enum class EPattyCookState : uint8
{
	Raw,
	Cooked,
	Overcooked
};

UCLASS()
class MHGA_API APatty : public AIngredientBase
{
	GENERATED_BODY()

/* Method */
public:
	APatty();
	
	// 외부(CookingArea)에서 이 패티가 구워졌는지 확인하는 함수
	UFUNCTION(BlueprintCallable, Category = "Patty")
	EPattyCookState GetCookState() const;

	// 패티의 상태를 변경하는 함수
	void SetCookState(const EPattyCookState NewState);

	UFUNCTION(Server, Reliable)
	void ServerSetCookState (const EPattyCookState NewState);

	UFUNCTION()
	void OnRep_CookState();
	
	void UpdateMaterial();

	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


/* Field */
public:
	UPROPERTY(ReplicatedUsing=OnRep_CookState)
	EPattyCookState CookState;	// 패티의 구워짐 상태

	

private:



	
};
