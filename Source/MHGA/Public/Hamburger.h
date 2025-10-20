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
	FString BurgerName;

	// 황규환이 추가함 햄버거 이름 받아오기용 함수
public:
	FString GetBurgerName();
	
};
