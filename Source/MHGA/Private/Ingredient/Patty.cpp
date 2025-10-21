
#include "Ingredient/Patty.h"


// Sets default values
APatty::APatty()
{
	PrimaryActorTick.bCanEverTick = true;

	IngType = EIngredient::RawPatty;
}

void APatty::BeginPlay()
{
	Super::BeginPlay();
	
}

void APatty::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

EPattyCookState APatty::GetCookState() const
{
	return CookState;
}

void APatty::SetCookState(EPattyCookState NewState)
{
	CookState = NewState;
	// TODO : 패티의 상태에 따라 추가 효과 넣기(연기 등)

	if ( CookState == EPattyCookState::Cooked )
		IngType = EIngredient::WellDonePatty;
	
	if ( CookState == EPattyCookState::Overcooked )
		IngType = EIngredient::OvercookedPatty;
}