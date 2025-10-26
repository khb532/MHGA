#include "MHGAGameInstance.h"

#include <string>

#include "MHGA.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Online/OnlineSessionNames.h"

void UMHGAGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* subsys = Online::GetSubsystem(GetWorld());
	if (subsys)
	{
		SessionInterface = subsys->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMHGAGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMHGAGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMHGAGameInstance::OnJoinSessionComplete);
			SessionInterface->OnEndSessionCompleteDelegates.AddUObject(this, &UMHGAGameInstance::OnEndSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMHGAGameInstance::OnDestroySessionComplete);
		}
	}
}

void UMHGAGameInstance::CreateMySession(FString displayName, int32 playerCount)
{
	//세선을 만들기 위한 옵션담을 변수
	FOnlineSessionSettings SessionSettings;
	//현재 사용중인 서브시스템 이름 - steam이면 steam 들어옴, null이면 null들어옴
	FName SubsystemName = Online::GetSubsystem(GetWorld())->GetSubsystemName();
	//서브시스템 이름이 NULL이면 Lan 이용하게 설정
	SessionSettings.bIsLANMatch = SubsystemName.IsEqual(FName(TEXT("NULL")));

	//**Steam에서 필수**
	//Lobby 사용 여부, 친구 상태 확인 여부
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;

	//세션 검색 허용 여부
	SessionSettings.bShouldAdvertise = true;
	//세션 최대 참여 인원 설정
	//TODO : 호스트는 public connections에 포함되지 않음
	SessionSettings.NumPublicConnections = playerCount - 1;
	displayName = StringBase64Encode(displayName);
	//커스텀 정보	- Key,Value값
	SessionSettings.Set(FName("NAME"), displayName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//sessionSettings 이용해서 세션 생성
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session interface invalid. Failed to create session."));
		return;
	}

	CurrentSessionName = FName(*displayName);
	bIsHostingSession = true;
	bPendingReturnToStart = false;

	FUniqueNetIdPtr netId = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	SessionInterface->CreateSession(*netId, CurrentSessionName, SessionSettings);
}

void UMHGAGameInstance::FindOtherSession()
{
	UE_LOG(LogTemp, Warning, TEXT("세션 조회 시작"));
	//sessionsearch 생성
	SessionSearch = MakeShared<FOnlineSessionSearch>();
	//현재 사용중인 서브시스템 이름 - steam이면 steam 들어옴, null이면 null들어옴
	FName subsystemName = Online::GetSubsystem(GetWorld())->GetSubsystemName();
	//서브시스템 이름이 NULL이면 Lan 이용하게 설정
	SessionSearch->bIsLanQuery = subsystemName.IsEqual(FName(TEXT("NULL")));
	//어떤 옵션을 기준으로 검색 - 스팀에선 꼭 필요
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	//커스텀 한 거로 찾고싶다 하면
	//sessionSearch->QuerySettings.Set(FName("DP_NAME"), FName("Wanted"), EOnlineComparisonOp::Equals);

	//검색 갯수 제한
	SessionSearch->MaxSearchResults = 100;
	//위 설정을 가지고 세션 조회
	SessionInterface->FindSessions(0,SessionSearch.ToSharedRef());
}

void UMHGAGameInstance::JoinOtherSession(int32 sessionIdx)
{
	//검색된 세션 결과물
	auto results = SessionSearch->SearchResults;
	//5.5 이후 바뀜 -> 아래 2개는 무조건 세팅한 것과 맞춰줘야됨
	results[sessionIdx].Session.SessionSettings.bUseLobbiesIfAvailable = true;
	results[sessionIdx].Session.SessionSettings.bUsesPresence = true;

	//세션 이름
	FString displayName;
	results[sessionIdx].Session.SessionSettings.Get(FName("NAME"), displayName);
	
	//세션 참여
	SessionInterface->JoinSession(0, FName(displayName), results[sessionIdx]);
}

void UMHGAGameInstance::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		CurrentSessionName = sessionName;
		bIsHostingSession = true;
		UE_LOG(LogTemp, Warning, TEXT("%s 세션 생성 성공"), *sessionName.ToString())

		//레벨 이동 - 정확한 경로 적어야됨, 생성자에서 한 것처럼 하면 안됨
		FString TravelURL = FString::Printf(TEXT("/Game/Maps/Lobby?listen?Nick=%s"), *NickName);
		GetWorld()->ServerTravel(TravelURL);
	}
	else
	{
		bIsHostingSession = false;
		CurrentSessionName = NAME_None;
		UE_LOG(LogTemp, Warning, TEXT("%s 세션 생성 실패"), *sessionName.ToString())
	}
}

void UMHGAGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("세션 조회 완료"));
	if (bWasSuccessful)
	{
		//검색된 세션 결과물
		auto results = SessionSearch->SearchResults;
		FindCompleteDelegate.ExecuteIfBound(results);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 조회 실패"));
		FindCompleteDelegate.ExecuteIfBound(SessionSearch->SearchResults);
	}
}

void UMHGAGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	PRINTLOG(TEXT("Load Finish"));
	
	//참여에 성공 했다면
	if (result == EOnJoinSessionCompleteResult::Success)
	{
		CurrentSessionName = sessionName;
		bIsHostingSession = false;
		bPendingReturnToStart = false;
		//서버가 만들어 놓은 세션 URL 얻기
		FString url;
		SessionInterface->GetResolvedConnectString(sessionName, url);
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *url);

		//서버가 있는 맵으로 이동 (최초에 한 번만)
		FString TravelURL = url + FString::Printf(TEXT("?Nick=%s"), *NickName);
		
		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		pc->ClientTravel(TravelURL, TRAVEL_Absolute);
	}
}


void UMHGAGameInstance::LeaveSessionAndReturnToStart()
{
	bPendingReturnToStart = true;

	if (!SessionInterface.IsValid() || CurrentSessionName.IsNone())
	{
		TravelBackToStartLevel();
		return;
	}

	if (!SessionInterface->GetNamedSession(CurrentSessionName))
	{
		TravelBackToStartLevel();
		return;
	}

	if (SessionInterface->EndSession(CurrentSessionName))
	{
		return;
	}

	if (SessionInterface->DestroySession(CurrentSessionName))
	{
		return;
	}

	TravelBackToStartLevel();
}

void UMHGAGameInstance::OnEndSessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (!bPendingReturnToStart)
	{
		return;
	}

	if (!SessionInterface.IsValid())
	{
		TravelBackToStartLevel();
		return;
	}

	if (!CurrentSessionName.IsNone() && sessionName != CurrentSessionName)
	{
		return;
	}

	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("EndSession failed for %s"), *sessionName.ToString());
	}

	if (!SessionInterface->DestroySession(sessionName))
	{
		TravelBackToStartLevel();
	}
}

void UMHGAGameInstance::OnDestroySessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (!bPendingReturnToStart)
	{
		return;
	}

	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("DestroySession failed for %s"), *sessionName.ToString());
	}

	TravelBackToStartLevel();
}

void UMHGAGameInstance::TravelBackToStartLevel()
{
	if (!bPendingReturnToStart)
	{
		return;
	}

	const bool bWasHosting = bIsHostingSession;
	bPendingReturnToStart = false;
	bIsHostingSession = false;
	CurrentSessionName = NAME_None;

	if (bWasHosting)
	{
		// if (UWorld* World = GetWorld())
		// {
		// 	const FString TravelURL = FString::Printf(TEXT("/Game/Maps/Start?listen?Nick=%s"), *NickName);
		// 	World->ServerTravel(TravelURL);
		// }
		const FString TravelOptions = FString::Printf(TEXT("listen?Nick=%s"), *NickName);                                                                                         
		UGameplayStatics::OpenLevel(this, FName(TEXT("/Game/Maps/Start")), true, TravelOptions);          
	}
	else
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
		{
			const FString TravelURL = FString::Printf(TEXT("/Game/Maps/Start?Nick=%s"), *NickName);
			PlayerController->ClientTravel(TravelURL, TRAVEL_Absolute);
		}
	}
}


FString UMHGAGameInstance::StringBase64Encode(FString str)
{
	//str을 std::string으로 변경
	std::string utf8str = TCHAR_TO_UTF8(*str);
	//utf-8 String을 uint8 Array로 변경
	TArray<uint8> arrayData = TArray<uint8>((uint8*)utf8str.c_str(), utf8str.length());

	return FBase64::Encode(arrayData);
}

FString UMHGAGameInstance::StringBase64Decode(FString str)
{
	TArray<uint8> arrayData;
	if (FBase64::Decode(str, arrayData))
	{
		std::string utf8str((char*)arrayData.GetData(), arrayData.Num());
		return UTF8_TO_TCHAR(utf8str.c_str());
		
	}
	return FString();
}
