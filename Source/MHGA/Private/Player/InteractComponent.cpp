
#include "Player/InteractComponent.h"
#include "MHGA.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Player/MHGACharacter.h"
#include "Camera/CameraComponent.h"
#include "GrabableProps.h"
#include "WrappingPaper.h"
#include "Ingredient/IngredientBase.h"
#include "ProfilingDebugging/CookStats.h"
#include "Props/IngContainer.h"

UInteractComponent::UInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_preview_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh"));
	m_preview_mesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	m_preview_mesh->SetVisibility(false);
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

	/* dove */
	if (bIsGrabbed && GrabbedProp)
	{
		FHitResult hitresult;
		FVector start = Owner->GetFirstPersonCameraComponent()->GetComponentLocation();
		FVector end = start + Owner->GetFirstPersonCameraComponent()->GetForwardVector() * 300.f;
		
		FCollisionQueryParams params;
		params.AddIgnoredActor(Owner);
		
		if (GetWorld()->LineTraceSingleByChannel(hitresult, start, end, ECollisionChannel::ECC_PhysicsBody, params))
			m_preview_mesh->SetWorldTransform(FTransform(hitresult.ImpactPoint));
		else
			m_preview_mesh->SetWorldTransform(FTransform(end));
		
	} /* dove */
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
		if (Cast<IGrabableProps>(Hit.GetActor()))
		{
			
			MulticastRPC_GrabProps(Hit);
		}
		else if (AIngContainer* Container = Cast<AIngContainer>(Hit.GetActor()))
		{
			AIngredientBase* Ing = Container->GetIngredient();
			MulticastRPC_InteractIngContainer(Ing);
		}
	}
}

void UInteractComponent::MulticastRPC_GrabProps_Implementation(FHitResult Hit)
{
	IGrabableProps* GrabInterface = Cast<IGrabableProps>(Hit.GetActor());
	GrabInterface->OnGrabbed(Owner);

	/*Hit.GetComponent()->SetSimulatePhysics(false);
	Hit.GetComponent()->SetCollisionProfileName(TEXT("Grabbed"));
	HoldDistance = FVector::Dist(Owner->GetFirstPersonCameraComponent()->GetComponentLocation(), Hit.GetActor()->GetActorLocation());
	HoldDistance = FMath::Clamp(HoldDistance, 50, 200);
	Cast<AActor>(GrabInterface)->AttachToComponent(Owner->GetFirstPersonCameraComponent(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GrabInterface->SetLocation(Owner->GetFirstPersonCameraComponent()->GetComponentLocation() + Owner->GetFirstPersonCameraComponent()->GetForwardVector() * HoldDistance);*/

	/* dove */
	AIngredientBase* pgrab = Cast<AIngredientBase>(Hit.GetActor());
	m_preview_mesh->SetStaticMesh(pgrab->GetMeshComp()->GetStaticMesh());
	m_preview_mesh->SetVisibility(true);
	pgrab->GetMeshComp()->SetVisibility(false);
	Cast<AIngredientBase>(Hit.GetActor())->GetMeshComp()->SetVisibility(false);/* dove */

	bIsGrabbed = true;
	GrabbedProp = GrabInterface;
}

void UInteractComponent::MulticastRPC_InteractIngContainer_Implementation(AIngredientBase* Ingredient)
{
	if (Ingredient)
	{
		Ingredient->OnGrabbed(Owner);
	
		UPrimitiveComponent* Comp = Ingredient->GetMeshComp();
		Comp->SetSimulatePhysics(false);
		Comp->SetCollisionProfileName(TEXT("Grabbed"));
		HoldDistance = 100.f;
		Ingredient->AttachToComponent(Owner->GetFirstPersonCameraComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Ingredient->SetLocation(Owner->GetFirstPersonCameraComponent()->GetComponentLocation() + Owner->GetFirstPersonCameraComponent()->GetForwardVector() * HoldDistance);

		bIsGrabbed = true;
		GrabbedProp = Ingredient;
	}
}

void UInteractComponent::PutProps()
{
	if (GrabbedProp)
	{
		/* dove */
		m_preview_mesh->SetVisibility(false);
		
		
		GrabbedProp->OnPut();

		AActor* P = Cast<AActor>(GrabbedProp);
		if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(P->GetRootComponent()))
		{
			PRINTINFO();
			Root->SetSimulatePhysics(true);
			Root->SetCollisionProfileName(TEXT("BlockAllDynamic"));
			if (AIngredientBase* p_ing = Cast<AIngredientBase>(P))
				p_ing->GetMeshComp()->SetVisibility(true);
			P->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			P->SetActorLocation(m_preview_mesh->GetComponentLocation());
			m_preview_mesh->SetStaticMesh(nullptr);
		}

		bIsGrabbed = false;
		GrabbedProp = nullptr;
	}
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