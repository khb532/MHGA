
#include "Ingredient/Patty.h"


// Sets default values
APatty::APatty()
{
	PrimaryActorTick.bCanEverTick = true;

	IngType = EIngredient::Patty;
}

void APatty::BeginPlay()
{
	Super::BeginPlay();
	
}

void APatty::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

