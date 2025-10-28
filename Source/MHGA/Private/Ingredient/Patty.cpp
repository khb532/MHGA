#include "Ingredient/Patty.h"
#include "Net/UnrealNetwork.h"


APatty::APatty()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	// 종합 상태 초기화
	IngType = EIngredient::RawPatty;
	CookState = EPattyCookState::Raw;

	// 각 면 상태 초기화
	bIsFrontCooked = false;
	bIsBackCooked = false;
	bIsFrontOverCooked = false;
	bIsBackOverCooked = false;

	// 기타 변수 초기화
	bIsFrontSideDown = true; // 기본값 : 앞면이 아래
}

void APatty::BeginPlay()
{
	Super::BeginPlay();

	
}

void APatty::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APatty::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APatty, CookState);
}

EPattyCookState APatty::GetCookState() const
{
	return CookState;
}

void APatty::ServerSetCookState_Implementation(const EPattyCookState NewState)
{
	CookState = NewState;
	
	if ( CookState == EPattyCookState::Cooked )
		IngType = EIngredient::WellDonePatty;
	
	if ( CookState == EPattyCookState::Overcooked )
		IngType = EIngredient::OvercookedPatty;

	OnRep_CookState();
}

void APatty::SetCookState(const EPattyCookState NewState)
{
	if (HasAuthority())
	{
		ServerSetCookState_Implementation(NewState);
	}
	else
	{
		ServerSetCookState(NewState);
	}
}

void APatty::OnRep_CookState()
{
	UpdateMaterial();
}

void APatty::UpdateMaterial()
{
	TObjectPtr<UStaticMeshComponent> pMesh = GetMeshComp();
	bool bShouldChangeMat = false;
	FLinearColor newcolor;
	if (pMesh != nullptr)
	{
		switch (CookState)
		{
		case EPattyCookState::Raw:
			bShouldChangeMat = false;
			break;

		case EPattyCookState::Cooked:
			bShouldChangeMat = true;
			newcolor = FLinearColor(0.125,0.038,0.023);
			break;

		case EPattyCookState::Overcooked:
			bShouldChangeMat = true;
			newcolor = FLinearColor::Black;
			break;
		}

		if (!bShouldChangeMat) return;
		UMaterialInstanceDynamic* DynamicMaterial =
		 UMaterialInstanceDynamic::Create(pMesh->GetMaterial(0), this);

		if (!DynamicMaterial) return;

		DynamicMaterial->SetVectorParameterValue(TEXT("CookingLevel"), newcolor);

		pMesh->SetMaterial(0, DynamicMaterial);
	}
}

//	TODO : 조리시작


//	TODO : 조리중지