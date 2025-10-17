#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "Tomato.generated.h"

UCLASS()
class MHGA_API ATomato : public AIngredientBase
{
	GENERATED_BODY()

public:
	ATomato();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
