// Fill out your copyright notice in the Description page of Project Settings.


#include "Counter/PickupZone.h"
#include "AI/CustomerManager.h" // 여기에 include 추가
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APickupZone::APickupZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(boxCollision);
	boxCollision->SetBoxExtent(FVector(100.f, 100.f, 20.f));
	boxCollision->SetCollisionProfileName(TEXT("Trigger"));

	boxCollision->OnComponentBeginOverlap.AddDynamic(this, &APickupZone::OnOverlapBegin);
	boxCollision->OnComponentEndOverlap.AddDynamic(this, &APickupZone::OnOverlapEnd);
}

// Called when the game starts or when spawned
void APickupZone::BeginPlay()
{
	Super::BeginPlay();
	manager = Cast<ACustomerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass()));
	if (!IsValid(manager))
	{
		UE_LOG(LogTemp, Error, TEXT("PickupZone이 Manager를 찾을 수 없습니다!"));
	}
}

// Called every frame
void APickupZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 겹친 액터가 음식(AFoodActor)인지 확인합니다.
	AHamburger* Food = Cast<AHamburger>(OtherActor);
	if (IsValid(Food) && !curHamburger)
	{
		UE_LOG(LogTemp, Log, TEXT("음식이 픽업 존에 놓였습니다: %s"), *Food->GetBurgerName());
		curHamburger = Food;

		// 손님 자동 호출
		// TODO : 손님 수동 호출 만들기
		// if (IsValid(manager))
		// {
		// 	manager->OnFoodPlacedInZone(this);
		// }
	}
}

void APickupZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == curHamburger)
	{
		curHamburger = nullptr;
	}
}

AHamburger* APickupZone::TakeFood()
{
	AHamburger* FoodToReturn = curHamburger;
	curHamburger = nullptr;
	return FoodToReturn;
}

bool APickupZone::HasFood() const
{
	return IsValid(curHamburger);
}

