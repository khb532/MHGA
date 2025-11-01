#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WrapperBox.generated.h"

UCLASS()
class MHGA_API AWrapperBox : public AActor
{
	GENERATED_BODY()

/* Method */
public:
	AWrapperBox();

	void SpawnWrapper();

	UFUNCTION(Netmulticast, Reliable)
	void MulticastRPC_WrapSound();

/* Field */
public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWrappingPaper> WrapperClass;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> WrapperPoint;

	UPROPERTY(EditAnywhere)
	USoundBase* WrapperSound;
};
