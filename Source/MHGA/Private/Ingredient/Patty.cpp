#include "Ingredient/Patty.h"
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
	bIsCooking = true;
	StartCookTimer();
}

void APatty::Server_StopCooking_Implementation()
{
	if (!bIsCooking) return;
	bIsCooking = false;
	StopAllTimer();
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
	if (bIsFrontOverCooked)
	{
		frontColor = FLinearColor::Black;
	}
	else if (bIsFrontCooked)
	{
		frontColor = FLinearColor(0.125, 0.038, 0.023);
	}
	else
	{
		frontColor = FLinearColor(1,1,1);
	}

	if (frontMaterial)
	{
		frontMaterial->SetVectorParameterValue(TEXT("CookingLevel"), frontColor);
	}

	// 뒷면 생상
	FLinearColor backColor;
	if (bIsBackOverCooked)
	{
		backColor = FLinearColor::Black;
	}
	else if (bIsBackCooked)
	{
		backColor = FLinearColor(0.125, 0.038, 0.023);
	}
	else
	{
		backColor = FLinearColor(1,1,1);
	}
	if (backMaterial)
	{
		backMaterial->SetVectorParameterValue(TEXT("CookingLevel"), backColor);
	}
}