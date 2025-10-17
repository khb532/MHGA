#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "MiddleBread.generated.h"

UCLASS()
class MHGA_API AMiddleBread : public AIngredientBase
{
	GENERATED_BODY()

public:
	AMiddleBread();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
