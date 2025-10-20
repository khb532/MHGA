#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IngContainer.generated.h"

class AIngredientBase;

UCLASS()
class MHGA_API AIngContainer : public AActor
{
	GENERATED_BODY()

public:
	AIngContainer();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AIngredientBase> Ingredient;

public:
	UFUNCTION()
	AIngredientBase* GetIngredient();
};
