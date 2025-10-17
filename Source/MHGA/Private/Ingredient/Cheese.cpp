
#include "Ingredient/Cheese.h"


// Sets default values
ACheese::ACheese()
{
	PrimaryActorTick.bCanEverTick = true;

	IngType = EIngredient::Cheese;
}

void ACheese::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACheese::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

