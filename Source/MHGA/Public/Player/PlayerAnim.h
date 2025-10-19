// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

class AMHGACharacter;
/**
 * 
 */
UCLASS()
class MHGA_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=PlayerAnim)
	float Speed = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=PlayerAnim)
	float Dir = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CameraRotX = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsCrouch = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=PlayerAnim)
	AMHGACharacter* OwnerPlayer;
};
