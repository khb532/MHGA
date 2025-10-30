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

	void PlayAlarm();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_PlayAlarm();

	/* Field */
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* boxComp;

	FTimerHandle h_FryTimerHandle;

	UPROPERTY(EditAnywhere)
	bool b_IsFryMachine = false;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundBase> p_AlarmSound;

	UPROPERTY(EditAnywhere)
	int32 m_nFryTime = 30;
};
