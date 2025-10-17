#include "Counter/CounterPOS.h"

#include "MHGA.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetComponent.h"
#include "Counter/CounterUI.h"
#include "Counter/CustomerButtonUI.h"
#include "Net/UnrealNetwork.h"


ACounterPOS::ACounterPOS()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(MeshComponent);
	ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (mesh.Succeeded())
		MeshComponent->SetStaticMesh(mesh.Object);
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(MeshComponent);
	ConstructorHelpers::FClassFinder<UCounterUI> cu(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WBP_Pos.WBP_Pos_C'"));
	if (cu.Succeeded())
		WidgetComponent->SetWidgetClass(cu.Class);
	
	WidgetComponent->SetRelativeLocation(FVector(50.0f, 0.0f, 110.0f));
	WidgetComponent->SetRelativeScale3D(FVector(0.1f));
	WidgetComponent->SetDrawSize(FVector2D(1920, 1080));

	bReplicates = true;
	bAlwaysRelevant = true;
	WidgetComponent->SetIsReplicated(true);
}

void ACounterPOS::BeginPlay()
{
	Super::BeginPlay();

	CounterUI = Cast<UCounterUI>(WidgetComponent->GetWidget());
	if (CounterUI)
		CounterUI->SetPosActor(this);
	
	// if (HasAuthority())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("[%s] Server has ACounterPOS"), *GetName());
	// }
	// else
	// 	UE_LOG(LogTemp, Warning, TEXT("[%s] Client has ACounterPOS"), *GetName());
}

void ACounterPOS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PrintNetLog();
}

void ACounterPOS::MulticastRPC_OnClickCustomerBtn_Implementation()
{
	PRINTINFO();
	
	CounterUI->OnClickCustomerBtnRPC();
}

void ACounterPOS::MulticastRPC_OnClickMenuBtn_Implementation()
{
	PRINTINFO();
	
	CounterUI->OnClickMenuBtnRPC();
}

void ACounterPOS::MulticastRPC_OrderMenuBtn_Implementation()
{
	CounterUI->OrderMenuBtnRPC();
}

void ACounterPOS::MulticastRPC_DeleteListBtn_Implementation()
{
	CounterUI->DeleteListBtnRPC();
}

void ACounterPOS::MulticastRPC_OnMenuReadyBtn_Implementation()
{
	CounterUI->OnMenuReadyBtnRPC();
}

void ACounterPOS::MulticastRPC_AddMenuToList_Implementation(const EBurgerMenu MenuName)
{
	CounterUI->AddMenuToListRPC(MenuName);
}

void ACounterPOS::MulticastRPC_CustomerOrderedMenu_Implementation(int32 CustomerNum)
{
	UCustomerButtonUI* TargetBtn = nullptr;
	const TArray<UWidget*>& C = CounterUI->GetCustomerGrid()->GetAllChildren();
	for (UWidget* Widget : C)
	{
		if (UCustomerButtonUI* Btn = Cast<UCustomerButtonUI>(Widget))
		{
			if (Btn->GetNum() == CustomerNum)
			{
				TargetBtn = Btn;
				break;
			}
		}
	}
	
	CounterUI->CustomerOrderedMenuRPC(TargetBtn);
}

void ACounterPOS::ServerRPC_SetCustomer_Implementation(ACustomerAI* Customer)
{
	CurrentCustomer = Customer;
}

void ACounterPOS::PrintNetLog()
{
	//net connection 상태
	FString connstr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	//Owner 상태
	FString ownerStr = GetOwner() != nullptr ? GetOwner()->GetActorNameOrLabel() : TEXT("Invalid Owner");
	//role
	// LOCAL - 서버든 클라든 내 입장에서의 Rold
	// REMOTE - 서버에서는 이 액터가 클라이언트에서 어떤 Role로 설정되어있나
	//			클라이언트에서는 이 액터가 서버에서 어떤 Role로 설정되어 있나
	FString roleStr = FString::Printf(TEXT("LOCAL : %s, REMOTE : %s"),
		*UEnum::GetValueAsString<ENetRole>(GetLocalRole()), *UEnum::GetValueAsString<ENetRole>(GetRemoteRole()));

	FString logStr = FString::Printf(TEXT("Connection : %s\r\nOwner : %s\r\nRole : %s"),
		*connstr, *ownerStr, *roleStr);
	DrawDebugString(GetWorld(), GetActorLocation(), logStr, nullptr, FColor::Yellow, 0, true);
}
