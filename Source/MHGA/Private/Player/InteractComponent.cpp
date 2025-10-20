
#include "Player/InteractComponent.h"
#include "MHGA.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Player/MHGACharacter.h"
#include "Camera/CameraComponent.h"
#include "GrabableProps.h"
#include "WrappingPaper.h"
#include "ProfilingDebugging/CookStats.h"

UInteractComponent::UInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AMHGACharacter>(GetOwner());
}

void UInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInteractComponent::GrabProps()
{
	FVector Start = Owner->GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector End = Start + Owner->GetFirstPersonCameraComponent()->GetForwardVector() * GrabDistance;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	if (GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity,
		ECollisionChannel::ECC_PhysicsBody, Sphere, Params))
	{
		if (AWrappingPaper* wp = Cast<AWrappingPaper>(Hit.GetActor()))
		{
			wp->TryWrap();
			return;
		}
		
		//PRINTLOG(TEXT("%s"), *Hit.GetActor()->GetActorNameOrLabel());
		MulticastRPC_GrabProps(Hit);
	}
}

void UInteractComponent::PutProps()
{
	if (GrabbedProp)
	{
		GrabbedProp->OnPut();

		AActor* P = Cast<AActor>(GrabbedProp);
		if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(P->GetRootComponent()))
		{
			PRINTINFO();
			Root->SetSimulatePhysics(true);
			Root->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			P->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}

		bIsGrabbed = false;
		GrabbedProp = nullptr;
	}
}


void UInteractComponent::MulticastRPC_GrabProps_Implementation(FHitResult Hit)
{
	IGrabableProps* GrabInterface = Cast<IGrabableProps>(Hit.GetActor());
	if (GrabInterface == nullptr)
		return;

	GrabInterface->OnGrabbed(Owner);

	Hit.GetComponent()->SetSimulatePhysics(false);
	Hit.GetComponent()->SetCollisionProfileName(TEXT("Grabbed"));
	HoldDistance = FVector::Dist(Owner->GetFirstPersonCameraComponent()->GetComponentLocation(), Hit.GetComponent()->GetComponentLocation());
	HoldDistance = FMath::Clamp(HoldDistance, 50, 200);
	Hit.GetActor()->AttachToComponent(Owner->GetFirstPersonCameraComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GrabInterface->SetLocation(Owner->GetFirstPersonCameraComponent()->GetComponentLocation() + Owner->GetFirstPersonCameraComponent()->GetForwardVector() * HoldDistance);
	
	bIsGrabbed = true;
	GrabbedProp = GrabInterface;
}


void UInteractComponent::MulticastRPC_PutProps_Implementation()
{
	PutProps();
}

void UInteractComponent::ServerRPC_InteractProps_Implementation()
{
	if (!bIsGrabbed)
		GrabProps();
	else
		MulticastRPC_PutProps();
}

void UInteractComponent::ServerRPC_UseProps_Implementation()
{
	if (bIsGrabbed && GrabbedProp)
	{
		GrabbedProp->OnUse();
	}
}