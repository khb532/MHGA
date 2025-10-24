#include "Hamburger.h"
#include "Net/UnrealNetwork.h"

AHamburger::AHamburger()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;

	BurgerName.Empty();

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetIsReplicated(true);
	SetReplicateMovement(true);
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

void AHamburger::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHamburger, BurgerName);
}

void AHamburger::SetLocation(FVector Loc)
{
	SetActorLocation(Loc);
}

void AHamburger::PrintLog()
{
	if (!BurgerName.IsEmpty())
	{
		FString Name = FString::Printf(TEXT("Burger Name : %s"), *BurgerName);
		DrawDebugString(GetWorld(), GetActorLocation(), Name, nullptr, FColor::Yellow, 0);
	}
	
}

void AHamburger::ServerSetName_Implementation(const FString& Name)
{
	BurgerName = Name;
}

void AHamburger::SetName(FString Name)
{
	if (HasAuthority())
		BurgerName = Name;
	else
		ServerSetName(Name);
}

FString AHamburger::GetBurgerName()
{
	return BurgerName;
}
