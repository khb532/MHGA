#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "Patty.generated.h"

UCLASS()
class MHGA_API APatty : public AIngredientBase
{
	GENERATED_BODY()

public:
	APatty();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
