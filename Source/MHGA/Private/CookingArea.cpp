#include "CookingArea.h"

#include "MHGA.h"
#include "Components/BoxComponent.h"
#include "Ingredient/Patty.h"

ACookingArea::ACookingArea()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(boxComp);

	if (HasAuthority())
	{
		boxComp->OnComponentBeginOverlap.AddDynamic(this, &ACookingArea::OnOverlapBegin);
		boxComp->OnComponentEndOverlap.AddDynamic(this, &ACookingArea::OnOverlapEnd);
	}
}

void ACookingArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PRINTINFO();
	if (!HasAuthority()) return;
	
	TObjectPtr<AIngredientBase> p_oningredient = Cast<AIngredientBase>(OtherActor);
	
	if (p_oningredient)
	{
		p_oningredient->StartCook();
		PRINTINFO();
	}
}

void ACookingArea::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	PRINTINFO();
	if (!HasAuthority()) return;
	TObjectPtr<AIngredientBase> p_oningredient = Cast<AIngredientBase>(OtherActor);
	
	if (p_oningredient)
	{
		p_oningredient->ShutdownCook();
		PRINTINFO();
	}
}
