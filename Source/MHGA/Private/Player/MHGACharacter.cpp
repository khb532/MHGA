// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/MHGACharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "MHGA.h"
#include "MHGAGameMode.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/WidgetInteractionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/InteractComponent.h"
#include "Player/MHGAPlayerController.h"
#include "Player/PlayerAnim.h"

AMHGACharacter::AMHGACharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(30.f, 90.0f);
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> sk(TEXT("/Script/Engine.SkeletalMesh'/Game/Asset/Character/Standing_W_Briefcase_Idle.Standing_W_Briefcase_Idle'"));
	if (sk.Succeeded())
		GetMesh()->SetSkeletalMesh(sk.Object);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	ConstructorHelpers::FClassFinder<UPlayerAnim> ani(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/Anim/ABP_Player.ABP_Player_C'"));
	if (ani.Succeeded())
		GetMesh()->AnimClass =ani.Class;

	FPSCamComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FPSCamComponent->SetupAttachment(GetMesh());
	FPSCamComponent->SetRelativeLocationAndRotation(FVector(0, -15, 20), FRotator(90, 90, 0));
	FPSCamComponent->bUsePawnControlRotation = true;
	FPSCamComponent->bEnableFirstPersonFieldOfView = true;
	FPSCamComponent->bEnableFirstPersonScale = true;
	FPSCamComponent->FirstPersonFieldOfView = 70.0f;
	FPSCamComponent->FirstPersonScale = 0.6f;

	//comps
	InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
	
	WidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteraction"));
	WidgetInteraction->SetupAttachment(FPSCamComponent);
	WidgetInteraction->InteractionDistance = 200.f;
	// 디버그 라인 표시 (테스트용)
	// WidgetInteraction->bShowDebug = true;
	// WidgetInteraction->DebugLineThickness = 0.1f;
	
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
	ConstructorHelpers::FObjectFinder<UInputAction> crouch(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Crouch.IA_Crouch'"));
	if (crouch.Succeeded())
		IA_Crouch = crouch.Object;
	ConstructorHelpers::FObjectFinder<UInputAction> start(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Start.IA_Start'"));
	if (crouch.Succeeded())
		IA_Start = start.Object;
	ConstructorHelpers::FObjectFinder<UInputAction> voice(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Voice.IA_Voice'"));
	if (voice.Succeeded())
		IA_Voice = voice.Object;
	ConstructorHelpers::FObjectFinder<UInputAction> ui(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_UI.IA_UI'"));
	if (ui.Succeeded())
		IA_UI = ui.Object;


	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	bReplicates = true;
}

void AMHGACharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled() == false) WidgetInteraction->Deactivate();
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
		EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Started, this, &AMHGACharacter::CrouchInput);
		EnhancedInputComponent->BindAction(IA_Start, ETriggerEvent::Started, this, &AMHGACharacter::StartInput);
		EnhancedInputComponent->BindAction(IA_Voice, ETriggerEvent::Started, this, &AMHGACharacter::StartVoiceInput);
		EnhancedInputComponent->BindAction(IA_Voice, ETriggerEvent::Completed, this, &AMHGACharacter::EndVoiceInput);
		EnhancedInputComponent->BindAction(IA_UI, ETriggerEvent::Started, this, &AMHGACharacter::StartUIInput);
		EnhancedInputComponent->BindAction(IA_UI, ETriggerEvent::Completed, this, &AMHGACharacter::EndUIInput);
	}
}

void AMHGACharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMHGACharacter, SkeletalMesh);
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
	if (IsLocallyControlled())
		InteractComponent->ServerRPC_InteractProps();
}

void AMHGACharacter::UseInput(const FInputActionValue& Value)
{
	if (IsLocallyControlled())
	{
		//3d ui interact press
		WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);

		//use prop
		InteractComponent->ServerRPC_UseProps();
	}
}

void AMHGACharacter::UseInputRelease(const FInputActionValue& Value)
{
	if (IsLocallyControlled())
		WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
}

void AMHGACharacter::CrouchInput(const FInputActionValue& Value)
{
	if (IsLocallyControlled())
	{
		if (!GetCharacterMovement()->IsCrouching())
			Crouch();
		else
			UnCrouch();
	}
}

void AMHGACharacter::StartInput(const FInputActionValue& Value)
{
	if (HasAuthority())
	{
		AMHGAGameMode* gm = Cast<AMHGAGameMode>(UGameplayStatics::GetGameMode(this));
		AMHGAGameState* gs = Cast<AMHGAGameState>(UGameplayStatics::GetGameState(this));
		if (gm && gs && !gs->bIsGamePlaying)
		{
			gm->GameStart();
		}
	}
}

void AMHGACharacter::StartVoiceInput()
{
	AMHGAPlayerController* pc = GetController<AMHGAPlayerController>();
	pc->StartTalking();
}

void AMHGACharacter::EndVoiceInput()
{
	AMHGAPlayerController* pc = GetController<AMHGAPlayerController>();
	pc->StopTalking();
}

void AMHGACharacter::StartUIInput()
{
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(pc);
	pc->SetShowMouseCursor(true);
}

void AMHGACharacter::EndUIInput()
{
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
	pc->SetShowMouseCursor(false);
}

void AMHGACharacter::OnRep_MeshChange()
{
	GetMesh()->SetSkeletalMesh(SkeletalMesh);
}

void AMHGACharacter::SetFirstPersonMesh(USkeletalMesh* inMesh)
{
	SkeletalMesh = inMesh;
	OnRep_MeshChange();
}
