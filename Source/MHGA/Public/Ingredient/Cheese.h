#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "Cheese.generated.h"

UCLASS()
class MHGA_API ACheese : public AIngredientBase
{
	GENERATED_BODY()

public:
	ACheese();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
