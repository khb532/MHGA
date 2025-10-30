#include "CookingArea.h"

#include "MHGA.h"
#include "Components/BoxComponent.h"
#include "Ingredient/Patty.h"
#include "Kismet/GameplayStatics.h"

ACookingArea::ACookingArea()
{
	PrimaryActorTick.bCanEverTick = true;
	
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(boxComp);

	if (HasAuthority())
	{
		boxComp->OnComponentBeginOverlap.AddDynamic(this, &ACookingArea::OnOverlapBegin);
		boxComp->OnComponentEndOverlap.AddDynamic(this, &ACookingArea::OnOverlapEnd);
	}

	bReplicates = true;
}

void ACookingArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	
	TObjectPtr<AIngredientBase> p_oningredient = Cast<AIngredientBase>(OtherActor);
	
	if (p_oningredient)
		p_oningredient->StartCook();

	if (b_IsFryMachine)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(h_FryTimerHandle))
		GetWorld()->GetTimerManager().SetTimer(h_FryTimerHandle, this, &ACookingArea::PlayAlarm, m_nFryTime, false);
	}
	
}

void ACookingArea::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;
	TObjectPtr<AIngredientBase> p_oningredient = Cast<AIngredientBase>(OtherActor);
	
	if (p_oningredient)
		p_oningredient->ShutdownCook();

	if (b_IsFryMachine)
		if (GetWorld()->GetTimerManager().IsTimerActive(h_FryTimerHandle))
			GetWorld()->GetTimerManager().ClearTimer(h_FryTimerHandle);
	
}

void ACookingArea::MulticastRPC_PlayAlarm_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_AlarmSound, this->GetActorLocation(), FRotator::ZeroRotator, 0.3);
}

void ACookingArea::PlayAlarm()
{
	MulticastRPC_PlayAlarm();
}

