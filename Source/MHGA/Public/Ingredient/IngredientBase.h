#pragma once

#include "CoreMinimal.h"
#include "BurgerData.h"
#include "GrabableProps.h"
#include "GameFramework/Actor.h"
#include "IngredientBase.generated.h"

class AMHGACharacter;
enum class EIngredient : uint8;

UCLASS()
class MHGA_API AIngredientBase : public AActor, public IGrabableProps
{
	GENERATED_BODY()

/* Method */
public:
	AIngredientBase();

	virtual void OnGrabbed(AMHGACharacter* Player) override;
	
	virtual void OnPut() override;
	
	virtual void OnUse() override;
	
	virtual void SetLocation(FVector Loc) override;
	
	EIngredient GetIngType() { return IngType; }
	
	virtual bool IsTool() override { return false; }

	virtual UStaticMeshComponent* GetMeshComp() override {return Mesh;}

	virtual void StartCook(){};
	virtual void ShutdownCook(){};

	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:

	
/* Field */
public:
		
protected:
	UPROPERTY(EditAnywhere, Category = "Ingredients")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Ingredients", Replicated)
	EIngredient IngType = EIngredient::None;

	UPROPERTY(EditAnywhere, Category = "Ingredients")
	AMHGACharacter* GrabCharacter;
	
};
