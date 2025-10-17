
#include "Ingredient/Pickle.h"


// Sets default values
APickle::APickle()
{
	PrimaryActorTick.bCanEverTick = true;

	IngType = EIngredient::Pickle;
}

void APickle::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

