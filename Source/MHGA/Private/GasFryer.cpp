// Fill out your copyright notice in the Description page of Project Settings.


#include "GasFryer.h"

// Sets default values
AGasFryer::AGasFryer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	fryer = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fryer"));
	SetRootComponent(fryer);
	basketL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("basket1"));
	basketL->SetupAttachment(fryer);
	basketR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("basket2"));
	basketR->SetupAttachment(fryer);
}

// Called when the game starts or when spawned
void AGasFryer::BeginPlay()
{
	Super::BeginPlay();

	upPosL = basketL->GetRelativeLocation();
	upPosR = basketR->GetRelativeLocation();

	SetActorTickEnabled(false);
}

// Called every frame
void AGasFryer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float curTime = GetWorld()->GetTimeSeconds();
	float elapsedTime = curTime - movementStartTime;
	float a = FMath::Clamp(elapsedTime / moveDuration, 0, 1);

	if (bIsMovingDown)
	{
		FVector newLocL = FMath::Lerp(upPosL, downPosL, a);
		FVector newLocR = FMath::Lerp(upPosR, downPosR, a);
		basketL->SetRelativeLocation(newLocL);
		basketR->SetRelativeLocation(newLocR);
		
		if (FMath::IsNearlyEqual(a, 1))
		{
			bIsMovingDown = false;
			SetActorTickEnabled(false);
		}
	}
	if (bIsMovingUp)
	{
		FVector newLocL = FMath::Lerp(downPosL, upPosL, a);
		FVector newLocR = FMath::Lerp(downPosR, upPosR, a);
		basketL->SetRelativeLocation(newLocL);
		basketR->SetRelativeLocation(newLocR);
		
		if (FMath::IsNearlyEqual(a, 1))
		{
			bIsMovingUp = false;
			SetActorTickEnabled(false);
		}
	}
}

void AGasFryer::StartCooking()
{
	if (!HasAuthority()) return;
	
	if (bIsMovingDown || bIsMovingUp || GetWorld()->GetTimerManager().IsTimerActive(cookTimer))
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		cookTimer,
		this,
		&AGasFryer::OnCookingFinished, // 타이머 만료 시 올라가라고 명령할 함수
		moveDuration + cookTime, // 총 시간
		false
	);

	// [핵심] 모든 클라이언트에게 "지금 내려가!"라고 명령
	Multicast_MovingDown();
}

void AGasFryer::OnCookingFinished()
{
	Multicast_MovingUp();
}

void AGasFryer::Multicast_MovingDown_Implementation()
{
	if (bIsMovingDown || bIsMovingUp) return; 

	bIsMovingDown = true;
	bIsMovingUp = false;
	movementStartTime = GetWorld()->GetTimeSeconds();
	SetActorTickEnabled(true); // 각자 자신의 Tick을 켬
}

void AGasFryer::Multicast_MovingUp_Implementation()
{
	bIsMovingDown = false;
	bIsMovingUp = true;
	movementStartTime = GetWorld()->GetTimeSeconds();
	SetActorTickEnabled(true);
}



