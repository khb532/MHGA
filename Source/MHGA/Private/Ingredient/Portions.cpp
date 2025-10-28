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

void APortions::StartCook()
{
	if (HasAuthority())
	{
		PRINTINFO();
		GetWorld()->GetTimerManager().SetTimer(h_CookTimer, this, &APortions::OnCookingComplete, 5.f, false);
	}
}

void APortions::ShutdownCook()
{
	if (HasAuthority())
	{
		PRINTINFO();
		GetWorld()->GetTimerManager().ClearTimer(h_CookTimer);
	}
}

void APortions::OnCookingComplete()
{
	PRINTINFO();
	if (!HasAuthority()) return;

	CookState = EPortionCookState::Cooked;

	if (b_IsShanghai && !b_IsShrimp)
		IngType = EIngredient::ShanghaiPortion;
	else if (b_IsShrimp && !b_IsShanghai)
		IngType = EIngredient::ShrimpPortion;
	else
		UE_LOG(LogTemp, Error, TEXT("Portions is Nan"))

	OnRep_CookState();
}

void APortions::OnRep_CookState()
{
	UpdateMaterial();
}

void APortions::UpdateMaterial()
{
	if (CookState != EPortionCookState::Cooked) return;

	TObjectPtr<UStaticMeshComponent> p_Mesh = GetMeshComp();
	if (!p_Mesh) return;

	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(p_Mesh->GetMaterial(0), this);
	if (!DynamicMaterial) return;

	if (b_IsShanghai && !b_IsShrimp)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("CookingLevel"), FLinearColor(0.125, 0.038, 0.023));
		p_Mesh->SetMaterial(0, DynamicMaterial);
	}
	else if (b_IsShrimp && !b_IsShanghai)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("CookingLevel"), FLinearColor(0.125, 0.038, 0.023));
		p_Mesh->SetMaterial(0, DynamicMaterial);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Portion is NaN"))
	}
}
