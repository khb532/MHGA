#include "Ingredient/IngredientBase.h"
#include "MHGA.h"
#include "Net/UnrealNetwork.h"
#include "Player/InteractComponent.h"
#include "Player/MHGACharacter.h"
#include "MHGAGameInstance.h"


AIngredientBase::AIngredientBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Mesh->SetEnableGravity(true);

	bReplicates = true;
	AActor::SetReplicateMovement(true);
}

void AIngredientBase::BeginPlay()
{
	Super::BeginPlay();

	GI = GetGameInstance<UMHGAGameInstance>();
	/*if (HasAuthority() == false)
	{
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
	}*/
}

void AIngredientBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIngredientBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIngredientBase, IngType);
}


void AIngredientBase::OnGrabbed(AMHGACharacter* Player)
{
	if (GrabCharacter != Player && GrabCharacter != nullptr)
	{
		PRINTINFO();
		GrabCharacter->GetInteractComponent()->PutProps();
	}

	GrabCharacter = Player;
	PRINTLOG(TEXT("GRAB!"));
}


void AIngredientBase::OnPut()
{
	GrabCharacter = nullptr;
	PRINTLOG(TEXT("PUT!"));
}

void AIngredientBase::OnUse()
{
	PRINTLOG(TEXT("USE!"));
}

void AIngredientBase::SetLocation(FVector Loc)
{
	SetActorLocation(Loc);
}

