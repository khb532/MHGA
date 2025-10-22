#pragma once

#include "CoreMinimal.h"
#include "GrabableProps.h"
#include "GameFramework/Actor.h"
#include "Hamburger.generated.h"

UCLASS()
class MHGA_API AHamburger : public AActor, public IGrabableProps
{
	GENERATED_BODY()

/* Method */
public:
	AHamburger();

	// TODO(human): 네트워크 복제를 위한 함수 선언 추가
	// 고려사항: BurgerName을 모든 클라이언트에 복제하기 위해 필요
	// 추가할 코드: virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnGrabbed(AMHGACharacter* Player) override;
	virtual void OnPut() override;
	virtual void OnUse() override;
	virtual void SetLocation(FVector Loc) override;

	void SetName(FString Name);
	
protected:
	virtual void BeginPlay() override;

private:
	void PrintLog();



/* Field */
public:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	bool bShowLog = true;
private:
	// TODO(human): BurgerName을 네트워크 복제되도록 설정
	// 고려사항: 서버에서 SetName()으로 설정한 이름이 클라이언트에도 보이려면 Replicated 필요
	// 수정 방법: FString BurgerName; → UPROPERTY(Replicated) FString BurgerName;
	FString BurgerName;

	// 황규환이 추가함 햄버거 이름 받아오기용 함수
public:
	FString GetBurgerName();
	
};
