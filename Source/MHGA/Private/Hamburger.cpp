#include "Hamburger.h"
#include "Components/BoxComponent.h"


AHamburger::AHamburger()
{
	PrimaryActorTick.bCanEverTick = true;

	BurgerName.Empty();

	Collision = CreateDefaultSubobject<UBoxComponent>(FName("Collision"));
	SetRootComponent(Collision);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	
}

void AHamburger::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHamburger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintLog();
}

void AHamburger::PrintLog()
{
	if (!BurgerName.IsEmpty())
	{
	FString ActorName = this->GetActorNameOrLabel();
		FString Name = FString::Printf(TEXT("Burger Name : %s"), *BurgerName);
		FString Str = FString::Printf(TEXT("%s\n"), *ActorName);
		Str += Name;
		DrawDebugString(GetWorld(), GetActorLocation(), Str, nullptr, FColor::Yellow, 0);
	}
	
}


void AHamburger::SetName(FString Name)
{
	BurgerName = Name;
}

