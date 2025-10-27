
#include "Props/WrapperBox.h"

#include "WrappingPaper.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"


AWrapperBox::AWrapperBox()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	WrapperPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Point"));
	WrapperPoint->SetupAttachment(Mesh);

	bReplicates = true;
}

void AWrapperBox::SpawnWrapper()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn Wrapper"))
	GetWorld()->SpawnActor<AWrappingPaper>(WrapperClass, WrapperPoint->GetComponentLocation(), FRotator(0));
}

