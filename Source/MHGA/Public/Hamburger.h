#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hamburger.generated.h"

UCLASS()
class MHGA_API AHamburger : public AActor
{
	GENERATED_BODY()

/* Method */
public:
	AHamburger();

	virtual void Tick(float DeltaTime) override;

	void SetName(FString Name);
	
protected:
	virtual void BeginPlay() override;

private:
	void PrintLog();



/* Field */
public:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Collision;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

private:
	FString BurgerName;
	
};
