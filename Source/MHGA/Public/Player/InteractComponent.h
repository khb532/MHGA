// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"


class AMHGACharacter;
class UPhysicsHandleComponent;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grab")
	UPhysicsHandleComponent* PhysicsHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grab")
	float GrabDistance = 200.f; // 집을 수 있는 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grab")
	float HoldDistance = 150.f; // 손에서 떨어진 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grab")
	float GrabRadius = 20.f; // 구체 트레이스 반경

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grab")
	bool bIsGrabbed = false;

protected:
	void GrabProps();
	void PutProps();
	
public:
	void InteractProps();
	void UseProps();
};
