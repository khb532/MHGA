#include "CookingArea.h"
#include "Components/BoxComponent.h"
#include "Ingredient/Patty.h"

ACookingArea::ACookingArea()
{
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(boxComp);

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ACookingArea::OnOverlapBegin);
	boxComp->OnComponentEndOverlap.AddDynamic(this, &ACookingArea::OnOverlapEnd);
}

void ACookingArea::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACookingArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACookingArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	// TODO : 튀김기에도 CookingArea 적용해야함. 패티기반으로만 되어있는걸 수정
	// TODO : 대상 재료 -> 조리 시작 호출
	/*	p_Patty >> p_IngredientBase
	 *	
	 */
	TObjectPtr<AIngredientBase> p_oningredient = Cast<AIngredientBase>(OtherActor);
	if (p_oningredient)
		p_oningredient->StartCook();
	
}

void ACookingArea::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;
	// TODO : 대상 재료의 조리중지 호출
	// p_Ing -> ShutdownCook();
	TObjectPtr<AIngredientBase> p_oningredient = Cast<AIngredientBase>(OtherActor);
	if (p_oningredient)
		// p_oningredient->ShutDownCook();
	
	// 영역을 나간 액터가 맵에 등록되어 있는지 확인합니다.
	if (overlapActorTimer.Contains(OtherActor))
	{
		// 맵에서 해당 액터의 타이머 핸들을 찾아 타이머를 중지시킵니다.
		GetWorld()->GetTimerManager().ClearTimer(overlapActorTimer[OtherActor]);
        
		// 맵에서 해당 액터를 제거합니다.
		overlapActorTimer.Remove(OtherActor);
	}
}

void ACookingArea::CookPatty(AActor* actor)
{
	if (actor == nullptr) return;

	APatty* PattyActor = Cast<APatty>(actor);
	if (PattyActor)
	{
		// 상태를 'Cooked'로 변경
		PattyActor->SetCookState(EPattyCookState::Cooked);
        
		//	머티리얼 색상을 '구워진' 색으로 변경
		//	ChangeColor(actor, cookedColor); // 기존 색 변경 로직을 재사용
		/*	ChanageColor => Patty::UpdateMaterial로 대체됨 (khb)	*/
		
		// 오버쿡 타이머를 새로 시작합니다.
		FTimerHandle overcookTimerHandle;
		FTimerDelegate overcookDelegate;
		overcookDelegate.BindUFunction(this, FName("OvercookPatty"), actor);
		GetWorld()->GetTimerManager().SetTimer(overcookTimerHandle, overcookDelegate, OvercookTime, false);
        
		// 기존 타이머 맵을 새 오버쿡 타이머로 업데이트하여 EndOverlap에서 멈출 수 있게 합니다.
		overlapActorTimer.Add(actor, overcookTimerHandle);
	}
}

void ACookingArea::OvercookPatty(AActor* actor)
{
	if (actor == nullptr) return;

	APatty* PattyActor = Cast<APatty>(actor);
	if (PattyActor)
	{
		// 1. 상태를 'Overcooked'로 변경
		PattyActor->SetCookState(EPattyCookState::Overcooked);

		/*// 2. 머티리얼 색상을 '오버쿡' 색(검은색)으로 변경
		ChangeColor(actor, overcookedColor);*/
	}
    
	// 모든 작업이 끝났으므로 맵에서 제거합니다.
	overlapActorTimer.Remove(actor);
}

