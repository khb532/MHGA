
#include "Props/WrapperBox.h"

#include "MHGAGameInstance.h"
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

	ConstructorHelpers::FObjectFinder<USoundBase> sound(TEXT("/Script/Engine.SoundCue'/Game/Asset/Sound/paper/PutPaper.PutPaper'"));
	if (sound.Succeeded())
		WrapperSound = sound.Object;

	bReplicates = true;
}

void AWrapperBox::SpawnWrapper()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn Wrapper"))
	GetWorld()->SpawnActor<AWrappingPaper>(WrapperClass, WrapperPoint->GetComponentLocation(), FRotator(0));
	MulticastRPC_WrapSound();
}

void AWrapperBox::MulticastRPC_WrapSound_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, WrapperSound, GetActorLocation(), 1, 1, 0,
	GetGameInstance<UMHGAGameInstance>()->SoundAttenuation);
}

