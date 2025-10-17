#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "Pickle.generated.h"

UCLASS()
class MHGA_API APickle : public AIngredientBase
{
	GENERATED_BODY()

public:
	APickle();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
