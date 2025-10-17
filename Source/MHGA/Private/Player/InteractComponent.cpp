
#include "Player/InteractComponent.h"
#include "MHGA.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Player/MHGACharacter.h"
#include "Camera/CameraComponent.h"
#include "GrabableProps.h"
#include "WrappingPaper.h"

UInteractComponent::UInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("HandleComponent"));
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

	if (PhysicsHandle->GrabbedComponent && bIsGrabbed)
	{
		FVector HoldLocation = Owner->GetFirstPersonCameraComponent()->GetComponentLocation() +
							   Owner->GetFirstPersonCameraComponent()->GetForwardVector() * HoldDistance;

		FRotator HoldRotation = Owner->GetFirstPersonCameraComponent()->GetComponentRotation();
		
		PhysicsHandle->SetTargetLocationAndRotation(HoldLocation, HoldRotation);
	}
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
			wp->CompleteWrapping();
			return;
		}
		
		//PRINTLOG(TEXT("%s"), *Hit.GetActor()->GetActorNameOrLabel());
		if (Hit.GetComponent()->IsSimulatingPhysics())
		{
			IGrabableProps* GrabInterface = Cast<IGrabableProps>(Hit.GetActor());
			if (GrabInterface == nullptr)
				return;
			
			UPrimitiveComponent* HitComp = Hit.GetComponent();
			HitComp->WakeAllRigidBodies();
			PhysicsHandle->GrabComponentAtLocationWithRotation(
				HitComp,
				NAME_None,
				Hit.ImpactPoint,
				Owner->GetFirstPersonCameraComponent()->GetComponentRotation()
			);

			bIsGrabbed = true;
			GrabbedProp = GrabInterface;
			GrabInterface->OnGrabbed();
		}
	}
	
}

void UInteractComponent::PutProps()
{	
	if (PhysicsHandle->GrabbedComponent)
	{
		GrabbedProp->OnPut();
		
		PhysicsHandle->ReleaseComponent();
		bIsGrabbed = false;
		GrabbedProp = nullptr;
	}
}

void UInteractComponent::InteractProps()
{
	if (!bIsGrabbed)
		GrabProps();
	else
		PutProps();
}

void UInteractComponent::UseProps()
{
	if (bIsGrabbed && GrabbedProp)
	{
		GrabbedProp->OnUse();
	}
}
