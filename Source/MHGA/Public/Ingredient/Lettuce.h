#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "Lettuce.generated.h"

UCLASS()
class MHGA_API ALettuce : public AIngredientBase
{
	GENERATED_BODY()

public:
	ALettuce();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
