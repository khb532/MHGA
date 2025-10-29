// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyGameMode.h"

#include "MHGA.h"
#include "MHGAGameInstance.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/LobbyGameState.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameFramework/PlayerController.h"
#include "Online/OnlineSessionNames.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
	GameSessionClass = AGameSession::StaticClass();
}

FString ALobbyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
                                      const FString& Options, const FString& Portal)
{
	FString Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	FString Nick = UGameplayStatics::ParseOption(Options, TEXT("Nick"));
	if (NewPlayerController && NewPlayerController->PlayerState && !Nick.IsEmpty())
	{
		NewPlayerController->PlayerState->SetPlayerName(Nick);
	}
	return Result;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PRINTINFO();
	if (ALobbyGameState* LGS = GetGameState<ALobbyGameState>())
	{
		if (APlayerState* PS = NewPlayer ? NewPlayer->PlayerState : nullptr)
			LGS->Server_AddPlayerName(PS->GetPlayerName());
	}
	else
	{
		PRINTINFO();
	}

	if (HasAuthority())
		UpdatePlayerCount();
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	if (ALobbyGameState* LGS = GetGameState<ALobbyGameState>())
	{
		if (APlayerState* PS = Exiting ? Exiting->PlayerState : nullptr)
			LGS->Server_RemovePlayerName(PS->GetPlayerName());
	}
	else
	{
		PRINTINFO();
	}

	Super::Logout(Exiting);
	
	if (HasAuthority())
		UpdatePlayerCount();
}

void ALobbyGameMode::UpdatePlayerCount()
{
	// 1. GameInstance 가져오기 및 Custom GameInstance로 캐스팅
	UMHGAGameInstance* GameInstance = Cast<UMHGAGameInstance>(GetGameInstance());
	if (!GameInstance) return;
    
	// 2. 현재 활성화된 세션 이름 가져오기 (가장 중요한 부분!)
	FName SessionToUpdate = GameInstance->GetCurrentSessionName();
	if (SessionToUpdate.IsNone()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 이름이 유효하지 않아 업데이트 불가."));
		return;
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	int32 NewPlayerCount = GetNumPlayers(); 
    
	FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(SessionToUpdate);
	FOnlineSessionSettings& NewSettings = CurrentSession->SessionSettings;

	if (NewSettings.Settings.Contains(FName("PlayerCount")))
	{
		NewSettings.Settings[FName("PlayerCount")].Data.SetValue(NewPlayerCount);
	}

	SessionInterface->UpdateSession(SessionToUpdate,NewSettings,true);
}
