// Copyright Epic Games, Inc. All Rights Reserved.


#include "Player/MHGAPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "MHGACameraManager.h"
#include "MHGAGameState.h"
#include "MHGAGameInstance.h"
#include "Counter/CounterPOS.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Lobby/LobbyBoard.h"
#include "Lobby/LobbyGameMode.h"
#include "Lobby/LobbyGameState.h"
#include "Player/MHGACharacter.h"

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
	
	if (AMHGAGameState* gs = Cast<AMHGAGameState>(GetWorld()->GetGameState()))
		CounterPos = gs->GetCounter();	

	if (ALobbyGameState* lgs = GetWorld()->GetGameState<ALobbyGameState>())
		LobbyBoard = lgs->GetBoard();
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


void AMHGAPlayerController::ServerRPC_Ready_Implementation(int32 PlayerNum)
{
	LobbyBoard->MulticastRPC_Ready(PlayerNum);
}

void AMHGAPlayerController::ServerRPC_Run_Implementation()
{
	LobbyBoard->MulticastRPC_Run();
}


void AMHGAPlayerController::Client_HandleGameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerController: Client_HandleGameOver 실행."));

	ACharacter* MyCharacter = GetCharacter();
	if (MyCharacter)
	{
		// 이동 컴포넌트 정지
		MyCharacter->GetCharacterMovement()->StopMovementImmediately();
		MyCharacter->GetCharacterMovement()->DisableMovement();

		// 입력 비활성화
		MyCharacter->DisableInput(this);
	}
}

void AMHGAPlayerController::ClientShowLoading_Implementation()
{
	if (UMHGAGameInstance* GI = Cast<UMHGAGameInstance>(GetGameInstance()))
	{
		GI->ShowLoadingScreen();
	}
}

void AMHGAPlayerController::ClientHideLoading_Implementation()
{
	if (UMHGAGameInstance* GI = Cast<UMHGAGameInstance>(GetGameInstance()))
	{
		GI->HideLoadingScreen();
	}
}