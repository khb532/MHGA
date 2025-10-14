
#include "Ingredient/TopBread.h"


// Sets default values
ATopBread::ATopBread()
{
	PrimaryActorTick.bCanEverTick = true;

	IngType = EIngredient::TopBread;
}

void ATopBread::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATopBread::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

