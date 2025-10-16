
#include "Ingredient/BottomBread.h"


// Sets default values
ABottomBread::ABottomBread()
{
	PrimaryActorTick.bCanEverTick = true;

	IngType = EIngredient::BottomBread;
}

void ABottomBread::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABottomBread::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

