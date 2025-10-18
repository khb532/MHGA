// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hamburger.h"
#include "GameFramework/Actor.h"
#include "PickupZone.generated.h"

// ADDED: ACustomerManager 라는 클래스가 있다고 컴파일러에게 미리 알려줍니다.
class ACustomerManager;
class UBoxComponent;
class AFoodActor;

UCLASS()
class MHGA_API APickupZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 음식이 들어왔는지 판정할 박스 콜리전
	UPROPERTY(EditAnywhere, Blueprintable)
	UBoxComponent* boxCollision;

	// 손님 매니저
	UPROPERTY()
	ACustomerManager* manager;

	// 현재 이 구역 안에 놓인 음식
	UPROPERTY()
	AHamburger* curHamburger;

	// 다른 액터가 이 콜리전 영역에 들어왔을 때 호출될 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	/** 구역 안의 음식을 가져갑니다. */
	AHamburger* TakeFood();

	/** 현재 구역 안에 음식이 있는지 확인합니다. */
	bool HasFood() const;

};
