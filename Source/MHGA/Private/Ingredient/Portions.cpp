#include "Ingredient/Portions.h"

#include "MHGA.h"
#include "Net/UnrealNetwork.h"


APortions::APortions()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	IngType = EIngredient::RawPortion;
	CookState = EPortionCookState::Raw;

}

void APortions::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APortions, CookState);
}

void APortions::BeginPlay()
{
	Super::BeginPlay();

	m_Material = UMaterialInstanceDynamic::Create(GetMeshComp()->GetMaterial(0), this);
	GetMeshComp()->SetMaterial(0, m_Material);
}

void APortions::StartCook()
{
	if (HasAuthority())
	{
		if (CookState == EPortionCookState::Cooked) return;
		GetWorld()->GetTimerManager().SetTimer(h_CookTimer, this, &APortions::OnCookingComplete, 5.f, false);
	}
	else
		ServerRPC_StartCook();
}

void APortions::ServerRPC_StartCook_Implementation()
{
	if (CookState == EPortionCookState::Cooked) return;
	GetWorld()->GetTimerManager().SetTimer(h_CookTimer, this, &APortions::OnCookingComplete, 5.f, false);
}

void APortions::ShutdownCook()
{
	if (HasAuthority())
		GetWorld()->GetTimerManager().ClearTimer(h_CookTimer);
	else
		ServerRPC_ShutdownCook();
}

void APortions::ServerRPC_ShutdownCook_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(h_CookTimer);
}

void APortions::OnCookingComplete()
{
	if (!HasAuthority()) return;

	CookState = EPortionCookState::Cooked;

	if (b_IsShanghai && !b_IsShrimp)
		IngType = EIngredient::ShanghaiPortion;
	else if (b_IsShrimp && !b_IsShanghai)
		IngType = EIngredient::ShrimpPortion;
	else
		PRINTLOG(TEXT("Portions is NaN"));

	UpdateMaterial();
}

void APortions::OnRep_CookState()
{
	if (CookState == EPortionCookState::Cooked)
		UpdateMaterial();
}

void APortions::UpdateMaterial()
{
	if (CookState != EPortionCookState::Cooked) return;

	if (b_IsShanghai && !b_IsShrimp)
	{
		m_Material->SetVectorParameterValue(TEXT("CookingLevel"), FLinearColor(0.25, 0.5, 0.3));
		GetMeshComp()->SetMaterial(0, m_Material);
	}
	else if (b_IsShrimp && !b_IsShanghai)
	{
		m_Material->SetVectorParameterValue(TEXT("CookingLevel"), FLinearColor(0.7, 0.35, 0.3));
		GetMeshComp()->SetMaterial(0, m_Material);
	}
	else
		PRINTLOG(TEXT("Portions is NaN"));
}
