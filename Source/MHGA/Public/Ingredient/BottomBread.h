#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "BottomBread.generated.h"

UCLASS()
class MHGA_API ABottomBread : public AIngredientBase
{
	GENERATED_BODY()

public:
	ABottomBread();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
