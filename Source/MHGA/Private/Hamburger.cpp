#include "Hamburger.h"
#include "Components/BoxComponent.h"


AHamburger::AHamburger()
{
	PrimaryActorTick.bCanEverTick = true;

	BurgerName.Empty();

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	
}

void AHamburger::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHamburger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShowLog)
		PrintLog();
}

void AHamburger::OnGrabbed(AMHGACharacter* Player)
{
	return;
}

void AHamburger::OnPut()
{
	return;
}

void AHamburger::OnUse()
{
	return;
}

void AHamburger::SetLocation(FVector Loc)
{
	SetActorLocation(Loc);
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

FString AHamburger::GetBurgerName()
{
	return BurgerName;
}
