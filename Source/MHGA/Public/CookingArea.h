#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CookingArea.generated.h"

UCLASS()
class MHGA_API ACookingArea : public AActor
{
	GENERATED_BODY()
	
public:	
	ACookingArea();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* boxComp;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


};
