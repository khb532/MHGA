#include "Props/IngContainer.h"

#include "Ingredient/IngredientBase.h"


AIngContainer::AIngContainer()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	bReplicates = true;
}

void AIngContainer::BeginPlay()
{
	Super::BeginPlay();
	
}

AIngredientBase* AIngContainer::GetIngredient()
{
	return GetWorld()->SpawnActor<AIngredientBase>(Ingredient);;
}
