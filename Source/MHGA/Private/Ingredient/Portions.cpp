#include "Ingredient/Portions.h"

#include "MHGA.h"
#include "MHGAGameInstance.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


APortions::APortions()
{
	PrimaryActorTick.bCanEverTick = true;

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
		GetWorld()->GetTimerManager().SetTimer(h_CookTimer, this, &APortions::OnCookingComplete, 15.f, false);
	}
	else
		ServerRPC_StartCook();
	
	MulticastRPC_PlayFrySfx();
}

void APortions::ServerRPC_StartCook_Implementation()
{
	if (CookState == EPortionCookState::Cooked) return;
	GetWorld()->GetTimerManager().SetTimer(h_CookTimer, this, &APortions::OnCookingComplete, 15.f, false);
}

void APortions::ShutdownCook()
{
	if (HasAuthority())
		GetWorld()->GetTimerManager().ClearTimer(h_CookTimer);
	else
		ServerRPC_ShutdownCook();

	MulticastRPC_StopFrySfx();
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
		m_Material->SetVectorParameterValue(TEXT("CookingLevel"), FLinearColor(0, 0, 0));
		GetMeshComp()->SetMaterial(0, m_Material);
	}
	else if (b_IsShrimp && !b_IsShanghai)
	{
		m_Material->SetVectorParameterValue(TEXT("CookingLevel"), FLinearColor(0, 0, 0));
		GetMeshComp()->SetMaterial(0, m_Material);
	}
	else
		PRINTLOG(TEXT("Portions is NaN"));
}


void APortions::MulticastRPC_PlayFrySfx_Implementation()
{
	if (m_sfx_FrySound)
	{
		if (m_sfx_FrySoundComp && m_sfx_FrySoundComp->IsPlaying())
			m_sfx_FrySoundComp->Stop();
		
		m_sfx_FrySoundComp = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), m_sfx_FrySound, this->GetActorLocation(), FRotator::ZeroRotator, 0.3,
			1, 0, GetGameInstance<UMHGAGameInstance>()->SoundAttenuation);
		
	}
}

void APortions::MulticastRPC_StopFrySfx_Implementation()
{
	if (m_sfx_FrySoundComp && m_sfx_FrySoundComp->IsPlaying())
		m_sfx_FrySoundComp->Stop();
}