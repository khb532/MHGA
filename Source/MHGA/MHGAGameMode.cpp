// Copyright Epic Games, Inc. All Rights Reserved.

#include "MHGAGameMode.h"

#include "MHGAGameInstance.h"
#include "MHGAGameState.h"
#include "AI/CustomerManager.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MHGACharacter.h"
#include "Player/MHGAPlayerController.h"

AMHGAGameMode::AMHGAGameMode()
{
	ConstructorHelpers::FClassFinder<AMHGACharacter> ch(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Player/BP_Player.BP_Player_C'"));
	if (ch.Succeeded())
	{
		DefaultPawnClass = ch.Class;
		characterList.Add(ch.Class);
	}
	ConstructorHelpers::FClassFinder<AMHGACharacter> ch2(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Player/BP_Player2.BP_Player2_C'"));
	if (ch2.Succeeded())
		characterList.Add(ch2.Class);
	ConstructorHelpers::FClassFinder<AMHGACharacter> ch3(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Player/BP_Player3.BP_Player3_C'"));
	if (ch3.Succeeded())
		characterList.Add(ch3.Class);
	ConstructorHelpers::FClassFinder<AMHGACharacter> ch4(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Player/BP_Player4.BP_Player4_C'"));
	if (ch4.Succeeded())
		characterList.Add(ch4.Class);
	
	
	PlayerControllerClass = AMHGAPlayerController::StaticClass();
	GameStateClass = AMHGAGameState::StaticClass();

	bUseSeamlessTravel = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AMHGAGameMode::BeginPlay()
{
	Super::BeginPlay();

	AMHGAGameState* gs = GetGameState<AMHGAGameState>();
	if (gs)
	{
		gs->remainTime = gs->startTime;
		gs->bIsGamePlaying = false;
		
		UE_LOG(LogTemp, Warning, TEXT("준비 완료, 대기중"));
	}
	
}

void AMHGAGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 서버에서만 실행
	if (!HasAuthority()) return;
	
	AMHGAGameState* gs = GetGameState<AMHGAGameState>();
	// gamestate가 없거나, 게임오버 상태거나, 게임이 아직 시작 안됐으면 함수 나가기
	if (!gs || gs->bIsGameOver || !gs->bIsGamePlaying) return;

	// 시간 업데이트
	gs->remainTime -= DeltaTime;

	// 게임 오버 조건 확인
	// 시간 초과
	if (gs->remainTime <= 0.f)
	{
		gs->remainTime = 0.f;
		HandleGameOver(TEXT("시간 초과"));
		return;
	}
	// TODO : 평점 관련 게임오버 필요시 추가
}

APawn* AMHGAGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
	const FTransform& SpawnTransform)
{

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);

	//플레이어가 선택한 캐릭터를 pawnclass에 설정
	FString playerName = NewPlayer->PlayerState->GetPlayerName();
	int32 charracterIdx = GetGameInstance<UMHGAGameInstance>()->GetSelectCharacter(playerName);
	if (charracterIdx != -1)
	{
		PawnClass = characterList[charracterIdx];
	}
	
	APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo);
	if (!ResultPawn)
	{
		UE_LOG(LogGameMode, Warning, TEXT("SpawnDefaultPawnAtTransform: Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
	}
	return ResultPawn;
}

// 평점 변경 요청 처리(서버)
void AMHGAGameMode::ReportScoreChanged(EScoreChangeReason reason, int32 changeScore)
{
	if (!HasAuthority()) return;
	AMHGAGameState* gs = GetGameState<AMHGAGameState>();
	if (!gs || gs->bIsGameOver) return;

	int32* targetScore = nullptr;

	// 이유에 따라 대상 평점 변수, 점수 설정
	switch (reason)
	{
	case EScoreChangeReason::SlowOrder:
		{
			targetScore = &gs->orderSpeedScore;
			changeScore = penaltySlowOrder;
		UE_LOG(LogTemp, Warning, TEXT("서버 : 평점 변경됨 (사유 : %d, 점수 : %d, 현재: 주문%d, 전달%d, 정확%d)"), (int32)reason, changeScore, gs->orderSpeedScore, gs->cookSpeedScore, gs->foodScore);
			
			break;
		}
	
	case EScoreChangeReason::SlowCook:
		{
			targetScore = &gs->cookSpeedScore;
			changeScore = penaltySlowCook;
		UE_LOG(LogTemp, Warning, TEXT("서버 : 평점 변경됨 (사유 : %d, 점수 : %d, 현재: 주문%d, 전달%d, 정확%d)"), (int32)reason, changeScore, gs->orderSpeedScore, gs->cookSpeedScore, gs->foodScore);
			
			break;
		}
	case EScoreChangeReason::WrongFood:
		{
			targetScore = &gs->foodScore;
			changeScore = penaltyWrongFood;
		UE_LOG(LogTemp, Warning, TEXT("서버 : 평점 변경됨 (사유 : %d, 점수 : %d, 현재: 주문%d, 전달%d, 정확%d)"), (int32)reason, changeScore, gs->orderSpeedScore, gs->cookSpeedScore, gs->foodScore);
			
			break;
		}
	case EScoreChangeReason::CorrectFood:
		{
			targetScore = &gs->foodScore;
			changeScore = bonusCorrectFood;
			UE_LOG(LogTemp, Warning, TEXT("서버 : 평점 변경됨 (사유 : %d, 점수 : %d, 현재: 주문%d, 전달%d, 정확%d)"), (int32)reason, changeScore, gs->orderSpeedScore, gs->cookSpeedScore, gs->foodScore);
			
			break;
		}
	}

	if (targetScore && changeScore != 0)
	{
		// 실제 평점 변경
		*targetScore += changeScore;

		// 평점 최소값 제한
		//*targetScore = FMath::Max(0, *targetScore);

		UE_LOG(LogTemp, Warning, TEXT("서버 : 평점 변경됨 (사유 : %d, 점수 : %d, 현재: 주문%d, 전달%d, 정확%d)"), (int32)reason, changeScore, gs->orderSpeedScore, gs->cookSpeedScore, gs->foodScore);
		
	}
}

void AMHGAGameMode::GameStart()
{
	AMHGAGameState* gs = GetGameState<AMHGAGameState>();
	ACustomerManager* customerManager = Cast<ACustomerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass()));
	if (gs && customerManager)
	{
		gs->bIsGamePlaying = true;
		gs->OnRep_GameStart();
		gs->remainTime = gs->startTime;
		UE_LOG(LogTemp, Warning, TEXT("서버 : 영업 시작, 제한시간 : %.1f초"), gs->remainTime);
		customerManager->StartSpawnCustomer();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode: 월드에서 CustomerManager를 찾을 수 없습니다!"));
	}
}

void AMHGAGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AMHGAPlayerController* PC = Cast<AMHGAPlayerController>(NewPlayer))
	{
		// 약간의 지연 후 로딩창 숨김
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [PC]()
		{
			if (PC)
			{
				PC->ClientHideLoading();
			}
		}, 3.0f, false); // 1초 후 숨김
	}
}

void AMHGAGameMode::HandleGameOver(FString reason)
{
	AMHGAGameState* gs = GetGameState<AMHGAGameState>();
	if (gs && !gs->bIsGameOver)
	{
		gs->bIsGameOver = true;
		UE_LOG(LogTemp, Error, TEXT("서버 : 영업 종료! 사유 : %s"), *reason);

		ACustomerManager* CustomerManager = Cast<ACustomerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass()));

		// 모든 손님을 퇴장시키라고 명령.
		if (CustomerManager)
		{
			CustomerManager->KickAllCustomers();
		}
		// TODO : 게임 종료 처리(플레이어 입력 중지, 결과화면 표시)
		gs->OnRep_GameOver();

		if (HasAuthority())
		{
			FTimerHandle handle;
			GetWorldTimerManager().SetTimer(handle, [this]()
			{
				GetWorld()->ServerTravel(TEXT("/Game/Maps/Lobby"), true);
			}, 10, false);
		}
	}
}
