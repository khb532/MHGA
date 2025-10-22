#include "Hamburger.h"
#include "Components/BoxComponent.h"
// TODO(human): 네트워크 복제를 위한 헤더 추가
// 추가할 코드: #include "Net/UnrealNetwork.h"


AHamburger::AHamburger()
{
	PrimaryActorTick.bCanEverTick = true;

	// TODO(human): 이 액터를 네트워크를 통해 복제되도록 설정
	// 문제: 현재 bReplicates = false (기본값)이므로 서버에서만 생성되고 클라이언트에는 보이지 않음
	// 해결: bReplicates = true; 추가
	// 영향: 서버에서 SpawnActor 시 모든 클라이언트에 자동으로 복제됨

	BurgerName.Empty();

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

}

void AHamburger::BeginPlay()
{
	Super::BeginPlay();

}

// TODO(human): GetLifetimeReplicatedProps 함수 구현 추가
// 목적: BurgerName 변수를 네트워크를 통해 복제
// 추가할 코드:
// void AHamburger::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
// {
//     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//     DOREPLIFETIME(AHamburger, BurgerName);
// }

void AHamburger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShowLog)
		PrintLog();
}

void AHamburger::OnGrabbed(AMHGACharacter* Player)
{
	return;
}

void AHamburger::OnPut()
{
	return;
}

void AHamburger::OnUse()
{
	return;
}

void AHamburger::SetLocation(FVector Loc)
{
	SetActorLocation(Loc);
}

void AHamburger::PrintLog()
{
	if (!BurgerName.IsEmpty())
	{
	FString ActorName = this->GetActorNameOrLabel();
		FString Name = FString::Printf(TEXT("Burger Name : %s"), *BurgerName);
		FString Str = FString::Printf(TEXT("%s\n"), *ActorName);
		Str += Name;
		DrawDebugString(GetWorld(), GetActorLocation(), Str, nullptr, FColor::Yellow, 0);
	}
	
}



void AHamburger::SetName(FString Name)
{
	BurgerName = Name;
}

FString AHamburger::GetBurgerName()
{
	return BurgerName;
}
