#include "Hamburger.h"

#include "BurgerData.h"
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

	TObjectPtr<UStaticMeshComponent> pMesh = GetMeshComp();
	if (pMesh)
	{
		frontMaterial = UMaterialInstanceDynamic::Create(pMesh->GetMaterial(0), this);
		pMesh->SetMaterial(0, frontMaterial);
		
	}

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

void AHamburger::SetMat(FString Name)
{
	if (Name == "WrongBurger")
	{
		frontMaterial->SetTextureParameterValue(TEXT("BurgerName"), wrongTexture);
	}
	else if (Name == "Shrimp")
	{
		frontMaterial->SetTextureParameterValue(TEXT("BurgerName"), shrimpTexture);
	}
	else if (Name == "BigMac")
	{
		frontMaterial->SetTextureParameterValue(TEXT("BurgerName"), bigMacTexture);
	}
}

FString AHamburger::GetBurgerName()
{
	return BurgerName;
}
