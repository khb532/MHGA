
#include "Ingredient/Onion.h"


// Sets default values
AOnion::AOnion()
{
	PrimaryActorTick.bCanEverTick = true;

	IngType = EIngredient::Onion;
}

void AOnion::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOnion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

