#include "CookingArea.h"

#include "MHGA.h"
#include "Components/BoxComponent.h"
#include "Ingredient/Patty.h"

ACookingArea::ACookingArea()
{
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(boxComp);

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ACookingArea::OnOverlapBegin);
	boxComp->OnComponentEndOverlap.AddDynamic(this, &ACookingArea::OnOverlapEnd);
}

void ACookingArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PRINTINFO();
	if (!HasAuthority()) return;
	// TODO : 튀김기에도 CookingArea 적용해야함. 패티기반으로만 되어있는걸 수정
	// TODO : 대상 재료 -> 조리 시작 호출
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
	// TODO : 대상 재료의 조리중지 호출
	// p_Ing -> ShutdownCook();
	TObjectPtr<AIngredientBase> p_oningredient = Cast<AIngredientBase>(OtherActor);
	
	if (p_oningredient)
	{
		p_oningredient->ShutdownCook();
		PRINTINFO();
	}
}