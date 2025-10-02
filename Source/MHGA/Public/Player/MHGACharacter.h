// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MHGACharacter.generated.h"

class UInteractComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AMHGACharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMHGACharacter();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

protected:
	//inputs
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* IA_Look;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* IA_Pick;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* IA_Use;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USkeletalMeshComponent* FirstPersonMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* FirstPersonCameraComponent;

	//comps
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UInteractComponent* InteractComponent;
	



protected:
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	void PickInput(const FInputActionValue& Value);
	void UseInput(const FInputActionValue& Value);


public:
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

