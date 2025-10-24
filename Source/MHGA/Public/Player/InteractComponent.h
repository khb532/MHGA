// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"


class AIngredientBase;
class AMHGACharacter;
class UPhysicsHandleComponent;
class IGrabableProps;
class UGrabableProps;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MHGA_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GrabC")
	AMHGACharacter* Owner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grab")
	float GrabDistance = 200.f; // 집을 수 있는 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grab")
	float HoldDistance = 150.f; // 손에서 떨어진 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grab")
	float GrabRadius = 20.f; // 구체 트레이스 반경

	/* dove */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> m_preview_mesh;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grab")
	bool bIsGrabbed = false;
	IGrabableProps* GrabbedProp;

protected:
	void GrabProps();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_GrabProps(FHitResult Hit);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_InteractIngContainer(AIngredientBase* Ingredient);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_PutProps();

public:
	void PutProps();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_InteractProps();
	UFUNCTION(Server, Reliable)
	void ServerRPC_UseProps();
};
