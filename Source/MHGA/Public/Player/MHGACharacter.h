#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MHGACharacter.generated.h"

class UWidgetInteractionComponent;
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
UCLASS()
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
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* IA_Crouch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* FPSCamComponent;

	//comps
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UInteractComponent* InteractComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UWidgetInteractionComponent* WidgetInteraction; //3d widget interact

protected:
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	void PickInput(const FInputActionValue& Value);
	void UseInput(const FInputActionValue& Value);
	void UseInputRelease(const FInputActionValue& Value);
	void CrouchInput(const FInputActionValue& Value);

public:
	USkeletalMeshComponent* GetFirstPersonMesh() const { return GetMesh(); }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FPSCamComponent; }
	UInteractComponent* GetInteractComponent() const {return InteractComponent;}
};

