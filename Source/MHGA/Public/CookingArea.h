#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CookingArea.generated.h"

UCLASS()
class MHGA_API ACookingArea : public AActor
{
	GENERATED_BODY()

	/* Method */
public:	
	ACookingArea();
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	/* Field */
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* boxComp;
};
