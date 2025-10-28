#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "Patty.generated.h"

// 패티의 요리 상태
UENUM(BlueprintType)
enum class EPattyCookState : uint8
{
	Raw,
	Cooked,
	Overcooked
};

UCLASS()
class MHGA_API APatty : public AIngredientBase
{
	GENERATED_BODY()

	/* Method */
public:
	APatty();
	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 패티 상태 변경시 호출되는 함수
	UFUNCTION()
	void OnRep_CookStateChanged();

	// 패티의 상태 변수
	UPROPERTY(ReplicatedUsing = OnRep_CookStateChanged)
	bool bIsFrontCooked;
	UPROPERTY(ReplicatedUsing = OnRep_CookStateChanged)
	bool bIsBackCooked;
	UPROPERTY(ReplicatedUsing = OnRep_CookStateChanged)
	bool bIsFrontOverCooked;
	UPROPERTY(ReplicatedUsing = OnRep_CookStateChanged)
	bool bIsBackOverCooked;

	// 패티의 종합 상태
	UPROPERTY(Replicated)
	EPattyCookState CookState;

	// 패티의 앞뒷면 확인 변수(앞면이 아래 = true)
	UPROPERTY(Replicated)
	bool bIsFrontSideDown;

	// 그릴 위에서 요리중인지 확인 변수
	UPROPERTY(Replicated)
	bool bIsCooking;

	// 구워짐 타이머
	FTimerHandle cookTimer;
	FTimerHandle overcookTimer;
	// 구워지는 시간 설정
	UPROPERTY(EditAnywhere)
	float cookTime = 3.f;
	UPROPERTY(EditAnywhere)
	float overcookTime = 5.f;

	// 앞뒷면 동적 머티리얼 인스턴스
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> frontMaterial;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> backMaterial;

	// CookingArea 호출 함수
	virtual void StartCook() override;
	virtual void ShutdownCook() override;

	void StartCooking();
	void StopCooking();
	// 뒤집게 함수
	void Flip();
	
	// 외부에서 패티의 종합 상태 확인하는 함수
	UFUNCTION(BlueprintCallable, Category = "Patty")
	EPattyCookState GetCookState() const;

	// 서버 RPC, 내부 함수
	UFUNCTION(Server, Reliable)
	void Server_StartCooking();
	UFUNCTION(Server, Reliable)
	void Server_StopCooking();
	UFUNCTION(Server, Reliable)
	void Server_Flip();

	// 서버 전용
	// 현재 상태에 맞춘 타이머 시작 함수
	void StartCookTimer();
	void StopAllTimer();
	// [타이머 콜백] 현재 면 상태 변경
	void CookCurrentSide();
	void OverCookCurrentSide();
	// 각 상태에 따라 종합 상태를 업데이트하는 함수
	void UpdateCookState();
	// 머티리얼 변경 함수
	void UpdateMaterial();
};
