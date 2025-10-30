#include "Ingredient/Patty.h"

#include "GeometryTypes.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


APatty::APatty()
{
	PrimaryActorTick.bCanEverTick = false;
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
	bIsCooking = false;

}

void APatty::BeginPlay()
{
	Super::BeginPlay();
	// 서버, 클라이언트 모두 머티리얼 인스턴스 미리 생성 후 저장
	TObjectPtr<UStaticMeshComponent> pMesh = GetMeshComp();
	if (pMesh)
	{
		// 슬롯0 : 앞면
		if (pMesh->GetMaterial(0))
		{
			frontMaterial = UMaterialInstanceDynamic::Create(pMesh->GetMaterial(0), this);
			pMesh->SetMaterial(0, frontMaterial);
		}
		// 슬롯1 : 뒷면
		if (pMesh->GetMaterial(1))
		{
			backMaterial = UMaterialInstanceDynamic::Create(pMesh->GetMaterial(1), this);
			pMesh->SetMaterial(1, backMaterial);
		}
	}

	// 초기 색상 적용
	UpdateMaterial();
}

void APatty::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APatty, CookState);
	DOREPLIFETIME(APatty, bIsFrontCooked);
	DOREPLIFETIME(APatty, bIsBackCooked);
	DOREPLIFETIME(APatty, bIsFrontOverCooked);
	DOREPLIFETIME(APatty, bIsBackOverCooked);
	DOREPLIFETIME(APatty, bIsFrontSideDown);
	DOREPLIFETIME(APatty, bIsCooking);
}

EPattyCookState APatty::GetCookState() const
{
	return CookState;
}

void APatty::StartCook()
{
	StartCooking();
}

void APatty::ShutdownCook()
{
	StopCooking();
}

void APatty::StartCooking()
{
	if (HasAuthority())
	{
		Server_StartCooking_Implementation();
	}
	else
	{
		Server_StartCooking();
	}
}

void APatty::StopCooking()
{
	if (HasAuthority())
	{
		Server_StopCooking_Implementation();
	}
	else
	{
		Server_StopCooking();
	}
}

void APatty::Flip()
{
	if (HasAuthority())
	{
		Server_Flip_Implementation();
	}
	else
	{
		Server_Flip();
	}
}

void APatty::Server_StartCooking_Implementation()
{
	if (bIsCooking) return;
	MulticastRPC_PlayGrillSfx();
	bIsCooking = true;
	Server_CheckFlip();
	GetWorldTimerManager().SetTimer(checkFlipTimer, this, &APatty::Server_CheckFlip, checkFlipTime, true);
	StartCookTimer();
}

void APatty::Server_StopCooking_Implementation()
{
	if (!bIsCooking) return;
	MulticastRPC_StopGrillSfx();
	bIsCooking = false;
	StopAllTimer();
	GetWorldTimerManager().ClearTimer(checkFlipTimer);
}

void APatty::Server_Flip_Implementation()
{
	// 앞뒷면 반전
	bIsFrontSideDown = !bIsFrontSideDown;
	// 요리중이면 타이머 멈추고 새로운면 타이머 시작
	if (bIsCooking)
	{
		StopAllTimer();
		StartCookTimer();
	}
	MulticastRPC_PlayGrillSfx();
}

void APatty::MulticastRPC_PlayGrillSfx_Implementation()
{
	if (m_sfx_GrillSound)
	{
		if (m_sfx_GrillSoundComp && m_sfx_GrillSoundComp->IsPlaying())
			m_sfx_GrillSoundComp->Stop();
		
		m_sfx_GrillSoundComp = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), m_sfx_GrillSound, this->GetActorLocation(), FRotator::ZeroRotator, 0.3);
		
	}
}

void APatty::MulticastRPC_StopGrillSfx_Implementation()
{
	if (m_sfx_GrillSoundComp && m_sfx_GrillSoundComp->IsPlaying())
		m_sfx_GrillSoundComp->Stop();
}

// 서버 전용
void APatty::StartCookTimer()
{
	if (!HasAuthority() || !bIsCooking) return;

	// 현재 아래로 향하고있는 면의 상태 확인
	bool bCurSideCooked = bIsFrontSideDown ? bIsFrontCooked : bIsBackCooked;
	bool bCurSideOverCooked = bIsFrontSideDown ? bIsFrontOverCooked : bIsBackOverCooked;

	if (!bCurSideCooked)
	{
		GetWorldTimerManager().SetTimer(cookTimer, this, &APatty::CookCurrentSide, cookTime, false);
	}
	if (!bCurSideOverCooked)
	{
		GetWorldTimerManager().SetTimer(overcookTimer, this, &APatty::OverCookCurrentSide, overcookTime, false);
	}
}

void APatty::StopAllTimer()
{
	GetWorldTimerManager().ClearTimer(cookTimer);
	GetWorldTimerManager().ClearTimer(overcookTimer);
}

// 서버전용 콜백, 현재 면 굽기
void APatty::CookCurrentSide()
{
	if (!HasAuthority()) return;
	if (bIsFrontSideDown)
	{
		bIsFrontCooked = true;
	}
	else
	{
		bIsBackCooked = true;
	}
	UpdateCookState();
	OnRep_CookStateChanged();
	StartCookTimer();	// 오버쿡 시작
}
// 현재 면 오버쿡
void APatty::OverCookCurrentSide()
{
	if (!HasAuthority()) return;
	if (bIsFrontSideDown)
	{
		bIsFrontOverCooked = true;
	}
	else
	{
		bIsBackOverCooked = true;
	}
	UpdateCookState();
	OnRep_CookStateChanged();
}

void APatty::UpdateCookState()
{
	if (!HasAuthority()) return;

	// 한쪽이라도 탔으면 오버쿡
	if (bIsFrontOverCooked || bIsBackOverCooked)
	{
		CookState = EPattyCookState::Overcooked;
		IngType = EIngredient::OvercookedPatty;
	}
	// 양쪽 모두 잘 구웠다
	else if (bIsFrontCooked && bIsBackCooked)
	{
		CookState = EPattyCookState::Cooked;
		IngType = EIngredient::WellDonePatty;
	}
	// 한쪽만 익었다, 혹은 둘다 안익었다
	else
	{
		CookState = EPattyCookState::Raw;
		IngType = EIngredient::RawPatty;
	}
}

void APatty::OnRep_CookStateChanged()
{
	UpdateMaterial();
}

void APatty::UpdateMaterial()
{
	// 앞면 색상
	FLinearColor frontColor;
	float frontRough = 1.f;
	TObjectPtr<UTexture2D> frontTexture = nullptr;
	TObjectPtr<UTexture2D> frontNormal= nullptr;
	TObjectPtr<UTexture2D> frontORM= nullptr;
	
	if (bIsFrontOverCooked)
	{
		frontColor = FLinearColor::Black;
		frontRough = 50.f;
		frontTexture = cookedTexture;
		frontNormal = cookedNormal;
		frontORM = cookedORM;
	}
	else if (bIsFrontCooked)
	{
		frontColor = FLinearColor(1, 1, 1);
		frontTexture = cookedTexture;
		frontNormal = cookedNormal;
		frontORM = cookedORM;
	}
	else
	{
		frontColor = FLinearColor(1,1,1);
		frontTexture = rawTexture;
		frontNormal = rawNormal;
		frontORM = rawORM;
	}

	if (frontMaterial)
	{
		frontMaterial->SetVectorParameterValue(TEXT("CookingLevel"), frontColor);
		frontMaterial->SetScalarParameterValue(TEXT("CookingRough"), frontRough);
		frontMaterial->SetTextureParameterValue(TEXT("BaseTexture"), frontTexture);
		frontMaterial->SetTextureParameterValue(TEXT("Normal"), frontNormal);
		frontMaterial->SetTextureParameterValue(TEXT("ORM"), frontORM);
	}

	// 뒷면 생상
	FLinearColor backColor;
	float backRough = 1.f;
	TObjectPtr<UTexture2D> backTexture = nullptr;
	TObjectPtr<UTexture2D> backNormal= nullptr;
	TObjectPtr<UTexture2D> backORM= nullptr;
	
	if (bIsBackOverCooked)
	{
		backColor = FLinearColor::Black;
		backRough = 50.f;
		backTexture = cookedTexture;
		backNormal = cookedNormal;
		backORM = cookedORM;
	}
	else if (bIsBackCooked)
	{
		backColor = FLinearColor(1, 1, 1);
		backTexture = cookedTexture;
		backNormal = cookedNormal;
		backORM = cookedORM;
	}
	else
	{
		backColor = FLinearColor(1,1,1);
		backTexture = rawTexture;
		backNormal = rawNormal;
		backORM = rawORM;
	}
	if (backMaterial)
	{
		backMaterial->SetVectorParameterValue(TEXT("CookingLevel"), backColor);
		backMaterial->SetScalarParameterValue(TEXT("CookingRough"), backRough);
		backMaterial->SetTextureParameterValue(TEXT("BaseTexture"), backTexture);
		backMaterial->SetTextureParameterValue(TEXT("Normal"), backNormal);
		backMaterial->SetTextureParameterValue(TEXT("ORM"), backORM);
	}
}

// 서버에서 앞뒤 상태 체크, 보정
void APatty::Server_CheckFlip()
{
	if (!HasAuthority() || !bIsCooking)
	{
		GetWorldTimerManager().ClearTimer(checkFlipTimer);
		return;
	}
	
	// 패티의 현재 위 방향 벡터
	FVector upVector = GetActorUpVector();

	// 월드의 위 방향 벡터와 내적
	float dot = FVector::DotProduct(upVector, FVector::UpVector);

	// 물리적으로 뒤집혀있는지 체크
	bool bIsPhysicallyFlipped = dot < 0.f;

	// 현재 로직 상태와 물리 상태 비교 함수
	bool bStateMismatch = false;
	
	if (bIsFrontSideDown && !bIsPhysicallyFlipped)
	{
		// 로직 : 앞면이 아래, 물리 : 뒷면이 아래 = 불일치
		bIsFrontSideDown = false;
		bStateMismatch = true;
		UE_LOG(LogTemp, Warning, TEXT("패티가 윗면"));
	}
	else if (!bIsFrontSideDown && bIsPhysicallyFlipped)
	{
		// 로직 : 앞면이 위, 물리 : 뒷면이 위 = 불일치
		bIsFrontSideDown = true;
		bStateMismatch = true;
		UE_LOG(LogTemp, Warning, TEXT("패티가 아랫면"));
	}

	// 상태가 불일치하여 보정되었을시
	if (bStateMismatch)
	{
		UE_LOG(LogTemp, Warning, TEXT("물리 상태 보정 감지. 새 방향으로 요리 타이머 재시작."));
		
		StopAllTimer();
		StartCookTimer();
	}
}
