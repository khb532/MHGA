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

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void AddIngredient(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(Server, Reliable)
	void RemoveIngredient(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// 포장 시도
	UFUNCTION(Server, Reliable)
	void TryWrap();
	
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
	
	bool HasBreadPair() const;
	
	bool HasExtraIngredient() const;
	
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



private:
	UPROPERTY(ReplicatedUsing=OnRep_AddIng)	// 충돌한 재료 저장
	TArray<FIngredientStack> OnAreaIngredients;

	UPROPERTY()		// 완료 시 제거할 액터 추적
	TArray<TWeakObjectPtr<AActor>> OverlappingActors;
	
	UPROPERTY(EditAnywhere)
	USoundBase* WrapperSound;
	UFUNCTION(Netmulticast, Reliable)
	void MulticastRPC_WrapSound();
};
