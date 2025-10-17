#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "Onion.generated.h"

UCLASS()
class MHGA_API AOnion : public AIngredientBase
{
	GENERATED_BODY()

public:
	AOnion();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
