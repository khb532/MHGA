// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/MHGACharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "MHGA.h"
#include "Components/WidgetInteractionComponent.h"
#include "Player/InteractComponent.h"

AMHGACharacter::AMHGACharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	//comps
	InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
	
	WidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteraction"));
	WidgetInteraction->SetupAttachment(FirstPersonCameraComponent);
	WidgetInteraction->InteractionDistance = 200.f;
	// 디버그 라인 표시 (테스트용)
	WidgetInteraction->bShowDebug = true;
	WidgetInteraction->DebugLineThickness = 0.1f;



	
	//////////////////////////////////Input///////////////////////////////
	ConstructorHelpers::FObjectFinder<UInputAction> move(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Move.IA_Move'"));
	if (move.Succeeded())
		IA_Move = move.Object;
	ConstructorHelpers::FObjectFinder<UInputAction> look(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Look.IA_Look'"));
	if (look.Succeeded())
		IA_Look = look.Object;
	ConstructorHelpers::FObjectFinder<UInputAction> pick(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Pick.IA_Pick'"));
	if (pick.Succeeded())
		IA_Pick = pick.Object;
	ConstructorHelpers::FObjectFinder<UInputAction> use(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Use.IA_Use'"));
	if (use.Succeeded())
		IA_Use = use.Object;
}

void AMHGACharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMHGACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMHGACharacter::MoveInput);
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMHGACharacter::LookInput);
		EnhancedInputComponent->BindAction(IA_Pick, ETriggerEvent::Started, this, &AMHGACharacter::PickInput);
		EnhancedInputComponent->BindAction(IA_Use, ETriggerEvent::Started, this, &AMHGACharacter::UseInput);
		EnhancedInputComponent->BindAction(IA_Use, ETriggerEvent::Completed, this, &AMHGACharacter::UseInputRelease);
	}
}

void AMHGACharacter::MoveInput(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (GetController())
	{
		AddMovementInput(GetActorRightVector(), MovementVector.X);
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	}
}

void AMHGACharacter::LookInput(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMHGACharacter::PickInput(const FInputActionValue& Value)
{
	InteractComponent->InteractProps();
}

void AMHGACharacter::UseInput(const FInputActionValue& Value)
{
	//3d ui interact press
	WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);

	//use prop
	InteractComponent->UseProps();
}

void AMHGACharacter::UseInputRelease(const FInputActionValue& Value)
{
	WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
}
