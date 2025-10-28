#include "Ingredient/Potions.h"


APortions::APortions()
{
	PrimaryActorTick.bCanEverTick = true;

	IngType = EIngredient::RawPortion;
}

void APortions::StartCook()
{
	//	Timer-> UpdateMaterial, UpdateIngType
	GetWorld()->GetTimerManager().SetTimer(h_CookTimer, this, &APortions::UpdateMaterial, 5.f, false, -1);
}

void APortions::ShutdownCook()
{
	//	Timer shutdown
	GetWorld()->GetTimerManager().ClearTimer(h_CookTimer);
}

void APortions::UpdateMaterial()
{
	TObjectPtr<UStaticMeshComponent> p_Mesh = GetMeshComp();
	if (!p_Mesh) return;
	
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(p_Mesh->GetMaterial(0), this);
	if (!DynamicMaterial) return;
	
	if (b_IsShanghai && !b_IsShrimp)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("CookingLevel"),FLinearColor(0.125,0.038,0.023));
		p_Mesh->SetMaterial(0, DynamicMaterial);
		UpdateIngType(EIngredient::ShanghaiPortion);
	}
	else if (b_IsShrimp && !b_IsShanghai)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("CookingLevel"),FLinearColor(0.125,0.038,0.023));
		p_Mesh->SetMaterial(0, DynamicMaterial);
		UpdateIngType(EIngredient::ShrimpPortion);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Portion is NaN"))
	}
}

void APortions::UpdateIngType(EIngredient ingtype)
{
	IngType = ingtype;
}
