#include "Ingredient/SauceBottle.h"

#include "MHGA.h"
#include "MHGAGameInstance.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"


ASauceBottle::ASauceBottle()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USoundBase> sound(TEXT("/Script/Engine.SoundCue'/Game/Asset/Sound/ketchup/Sauce.Sauce'"));
	if (sound.Succeeded())
		SauceSound = sound.Object;

	IngType = EIngredient::None;

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(GetRootComponent());
	Arrow->SetRelativeLocation(FVector(0, 0, 30));
	Arrow->SetRelativeRotation(FRotator(90, 0, 0));
}

void ASauceBottle::BeginPlay()
{
	Super::BeginPlay();

	
}

void ASauceBottle::OnGrabbed(AMHGACharacter* Player)
{
	Super::OnGrabbed(Player);

	bGrabbed = true;
}

void ASauceBottle::OnPut()
{
	Super::OnPut();

	bGrabbed = false;
}

void ASauceBottle::OnUse()
{
	Super::OnUse();

	ShootSauce();
	
}

void ASauceBottle::MulticastRPC_PlaySauceSound_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, SauceSound, GetActorLocation(),
		1, 1, 0, GI->SoundAttenuation);
}

void ASauceBottle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bGrabbed)
		this->SetActorRotation(FRotator(-180, 0 ,0));
}

void ASauceBottle::ShootSauce()
{
	if (bShowLog)
		PRINTLOG(TEXT("Shoot"));
	
	FHitResult HitResult;
	FVector StartLoc = Arrow->GetComponentLocation();
	FVector EndLoc = StartLoc + GetActorUpVector() * 200.f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bool bShot = GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECollisionChannel::ECC_PhysicsBody, Params);
	if (bShot && SauceClass)
	{
		if (bShowLog)
		{
			FString name = FString::Printf(TEXT("Hit Actor : %s"), *HitResult.GetActor()->GetActorNameOrLabel());
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, name, true);
		}
		FVector SpawnLoc = HitResult.ImpactPoint + FVector(0, 0, 20);
		UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), SpawnLoc.X, SpawnLoc.Y, SpawnLoc.Z);
		auto Sauce = GetWorld()->SpawnActor(SauceClass, &SpawnLoc, &FRotator::ZeroRotator);
		MulticastRPC_PlaySauceSound();
	}
}
