#pragma once

#include "CoreMinimal.h"
#include "BurgerData.h"
#include "GameFramework/Actor.h"
#include "WrappingPaper.generated.h"

UCLASS()
class MHGA_API AWrappingPaper : public AActor
{
	GENERATED_BODY()

/* Method */
public:
	AWrappingPaper();

	// TODO(human): Listen 서버 멀티플레이 구현
	// 고려사항: 서버만 게임 로직 실행(Authority), 클라이언트는 서버에 요청(RPC), 상태는 자동 동기화(Replication)
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void AddIngredient(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(Server, Reliable)
	void RemoveIngredient(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	

	// 포장 시도
	UFUNCTION(Server, Reliable)
	void TryWrap();
	
	// 레시피 매칭 결과로 햄버거를 스폰하고 포장지를 정리한다.
	void CompleteWrapping();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// 배열을 맵으로 변환
	TMap<EIngredient, int32> MakeMapFromArray(const TArray<FIngredientStack>& InArray);

	// Data테이블의 레시피 매칭
	// 성공하면 EBurgerMenu BurgerName 반환, 실패하면 EBurgerMenu::WrongBurger
	EBurgerMenu FindMatchingRecipe(UDataTable* DT, const TArray<FIngredientStack>& WrapperIngr);
	
	// 현재 재료 목록에 Top/Bottom Bread가 모두 존재하는지 확인한다.
	bool HasBreadPair() const;
	// 빵 이외 재료가 최소 하나 이상인지 확인한다.
	bool HasExtraIngredient() const;
	
	// 포장지 위 재료 액터를 파괴하고 내부 상태를 초기화한다.
	void DestroyIngredients();

	void PrintLog();

	UFUNCTION()
	void OnRep_AddIng();
	
/* Field */
public:
	UPROPERTY(EditAnywhere)
	class UDataTable* BurgerDataTable;

	UPROPERTY(EditAnywhere)
	class USceneComponent* Root;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* Collision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Debug)
	bool bShowLog = true;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHamburger> BurgerClass;

protected:


private:
	UPROPERTY(ReplicatedUsing=OnRep_AddIng)	// 충돌한 재료 저장
	TArray<FIngredientStack> OnAreaIngredients;

	UPROPERTY()		// 완료 시 제거할 액터 추적
	TArray<TWeakObjectPtr<AActor>> OverlappingActors;

	
};
