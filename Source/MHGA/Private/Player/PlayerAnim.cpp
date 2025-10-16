// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerAnim.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Player/MHGACharacter.h"

void UPlayerAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UPlayerAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerPlayer = Cast<AMHGACharacter>(TryGetPawnOwner());
}

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwnerPlayer)
	{
		Speed = FVector::DotProduct(OwnerPlayer->GetVelocity(), OwnerPlayer->GetActorForwardVector())/OwnerPlayer->GetCharacterMovement()->GetMaxSpeed();
		Dir = FVector::DotProduct(OwnerPlayer->GetVelocity(), OwnerPlayer->GetActorRightVector())/OwnerPlayer->GetCharacterMovement()->GetMaxSpeed();

		CameraRotX = -OwnerPlayer->GetBaseAimRotation().Pitch;
	}
}
