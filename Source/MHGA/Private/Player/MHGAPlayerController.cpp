// Copyright Epic Games, Inc. All Rights Reserved.


#include "Player/MHGAPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "MHGACameraManager.h"
#include "MHGAGameState.h"
#include "Counter/CounterPOS.h"

AMHGAPlayerController::AMHGAPlayerController()
{
	PlayerCameraManagerClass = AMHGACameraManager::StaticClass();

	ConstructorHelpers::FObjectFinder<UInputMappingContext> imc(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Player.IMC_Player'"));
	if (imc.Succeeded())
		IMC = imc.Object;
}

void AMHGAPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	AMHGAGameState* gs = Cast<AMHGAGameState>(GetWorld()->GetGameState());
	CounterPos = gs->GetCounter();
}

void AMHGAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
			Subsystem->AddMappingContext(IMC, 0);
	}
	
}

void AMHGAPlayerController::ServerRPC_OnClickCustomerBtn_Implementation()
{
	CounterPos->MulticastRPC_OnClickCustomerBtn();
}

void AMHGAPlayerController::ServerRPC_OnClickMenuBtn_Implementation()
{
	CounterPos->MulticastRPC_OnClickMenuBtn();
}

void AMHGAPlayerController::ServerRPC_OrderMenuBtn_Implementation()
{
	CounterPos->MulticastRPC_OrderMenuBtn();
}

void AMHGAPlayerController::ServerRPC_DeleteListBtn_Implementation()
{
	CounterPos->MulticastRPC_DeleteListBtn();
}

void AMHGAPlayerController::ServerRPC_OnMenuReadyBtn_Implementation()
{
	CounterPos->MulticastRPC_OnMenuReadyBtn();
}

void AMHGAPlayerController::ServerRPC_AddMenuToList_Implementation(const EBurgerMenu MenuName)
{
	CounterPos->MulticastRPC_AddMenuToList(MenuName);
}

void AMHGAPlayerController::ServerRPC_CustomerOrderedMenu_Implementation(int32 CustomerNum)
{
	CounterPos->MulticastRPC_CustomerOrderedMenu(CustomerNum);
}
