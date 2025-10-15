// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MHGACharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS()
class AMHGACharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* IA_Look;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* IA_Pick;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* IA_Use;
	
public:
	AMHGACharacter();

protected:
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	void PickInput(const FInputActionValue& Value);
	void UseInput(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

public:
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

