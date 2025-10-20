#include "Props/Flipper.h"

#include "MHGA.h"
#include "Camera/CameraComponent.h"
#include "Player/InteractComponent.h"
#include "Player/MHGACharacter.h"


AFlipper::AFlipper()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("/Script/Engine.StaticMesh'/Game/Asset/Mesh/Kitchen_Equipment/SM_Burger_Flipper/StaticMeshes/SM_Burger_Flipper.SM_Burger_Flipper'"));
	if (mesh.Succeeded())
		Mesh->SetStaticMesh(mesh.Object);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);

	bReplicates = true;
	AActor::SetReplicateMovement(true);
}

void AFlipper::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFlipper::OnGrabbed(AMHGACharacter* Player)
{
	if (GrabCharacter != Player && GrabCharacter != nullptr)
	{
		PRINTINFO();
		GrabCharacter->GetInteractComponent()->PutProps();
	}

	GrabCharacter = Player;

	SetActorRotation(Player->GetFirstPersonCameraComponent()->GetComponentRotation());
	PRINTLOG(TEXT("GRAB!"));
}


void AFlipper::OnPut()
{
	GrabCharacter = nullptr;
	PRINTLOG(TEXT("PUT!"));
}

void AFlipper::OnUse()
{
	FVector Start = GrabCharacter->GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector End = Start + GrabCharacter->GetFirstPersonCameraComponent()->GetForwardVector() * 200;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(20.f);
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GrabCharacter);

	if (GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity,
		ECollisionChannel::ECC_PhysicsBody, Sphere, Params))
	{
		if (Hit.GetComponent()->IsSimulatingPhysics())
		{
			IGrabableProps* GrabInterface = Cast<IGrabableProps>(Hit.GetActor());
			if (GrabInterface == nullptr)
				return;

			
			// 플레이어의 시선 기준 회전 행렬 생성
			FRotationMatrix CamMatrix(GrabCharacter->GetFirstPersonCameraComponent()->GetComponentRotation());
			// 시선의 Right 벡터(플레이어 기준 좌우 축)
			FVector RightVector = CamMatrix.GetScaledAxis(EAxis::Y);

			// Hit된 액터의 현재 회전
			FQuat CurrentQuat = Hit.GetActor()->GetActorQuat();
			// RightVector 축으로 180도 회전 쿼터니언 생성
			FQuat FlipQuat = FQuat(RightVector, FMath::DegreesToRadians(180.f));
			// 최종 회전 = 기존 회전 * 반전 회전
			FQuat NewQuat = FlipQuat * CurrentQuat;

			Hit.GetActor()->SetActorRotation(NewQuat);
		}
	}
	
	
	PRINTLOG(TEXT("USE!"));
}


