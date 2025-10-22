#include "Lobby/LobbyBoard.h"

#include "Components/WidgetComponent.h"
#include "Lobby/LobbyUI.h"

ALobbyBoard::ALobbyBoard()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(MeshComponent);
	ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("/Script/Engine.StaticMesh'/Game/Asset/Mesh/Furniture_and_Props/SM_Staff_Sign/StaticMeshes/SM_Staff_Sign.SM_Staff_Sign'"));
	if (mesh.Succeeded())
		MeshComponent->SetStaticMesh(mesh.Object);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(MeshComponent);
	ConstructorHelpers::FClassFinder<ULobbyUI> lo(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Lobby/WBP_LobbyUI.WBP_LobbyUI_C'"));
	if (lo.Succeeded())
		WidgetComponent->SetWidgetClass(lo.Class);

	WidgetComponent->SetRelativeLocation(FVector(54.25f,10,159.75f));
	WidgetComponent->SetRelativeRotation(FRotator(0,90,0));
	WidgetComponent->SetRelativeScale3D(FVector(1, 0.022f,0.0282f));
	WidgetComponent->SetDrawSize(FVector2D(1920, 1080));

	bReplicates = true;
	bAlwaysRelevant = true;
	WidgetComponent->SetIsReplicated(true);
}

void ALobbyBoard::BeginPlay()
{
	Super::BeginPlay();

	LobbyUI = Cast<ULobbyUI>(WidgetComponent->GetWidget());
	if (LobbyUI)
		LobbyUI->Init(this);
}

void ALobbyBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

