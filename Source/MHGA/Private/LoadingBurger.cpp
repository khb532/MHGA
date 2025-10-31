#include "LoadingBurger.h"

#include "Components/SceneCaptureComponent2D.h"


ALoadingBurger::ALoadingBurger()
{
	PrimaryActorTick.bCanEverTick = true;

	TObjectPtr<USceneComponent> Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);
	Mesh->SetRelativeLocation(FVector(0, 0, 0));

	Capture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));
	Capture->SetupAttachment(Root);
	Capture->SetRelativeLocation(FVector(-200, 0, 0));
	Capture->SetRelativeRotation(FRotator::ZeroRotator);
	Capture->ProjectionType = ECameraProjectionMode::Type::Perspective;
	Capture->FOVAngle = 60.f;
	Capture->PrimitiveRenderMode =	ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	Capture->ShowOnlyComponents.Add(Mesh);
}

void ALoadingBurger::BeginPlay()
{
	Super::BeginPlay();

	if (RenderTarget)
		Capture->TextureTarget = RenderTarget;
}

void ALoadingBurger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator rot = Mesh->GetRelativeRotation();
	rot.Yaw += RotationSpeed * DeltaTime;
	Mesh->SetRelativeRotation(rot);
}

