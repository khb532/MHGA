// Copyright Epic Games, Inc. All Rights Reserved.

#include "MHGAGameMode.h"

#include "MHGAGameState.h"
#include "Player/MHGACharacter.h"
#include "Player/MHGAPlayerController.h"

AMHGAGameMode::AMHGAGameMode()
{
	ConstructorHelpers::FClassFinder<AMHGACharacter> ch(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Player/BP_Player.BP_Player_C'"));
	if (ch.Succeeded())
		DefaultPawnClass = ch.Class;
	PlayerControllerClass = AMHGAPlayerController::StaticClass();
	GameStateClass = AMHGAGameState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
}

void AMHGAGameMode::BeginPlay()
{
	Super::BeginPlay();

	AMHGAGameState* gs = GetGameState<AMHGAGameState>();
	if (gs)
	{
		gs->remainTime = gs->startTime;
		UE_LOG(LogTemp, Warning, TEXT("서버 : 영업 시작, 제한시간 : %.1f초"), gs->remainTime);
	}
}

void AMHGAGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 서버에서만 실행
	if (!HasAuthority()) return;
	
	AMHGAGameState* gs = GetGameState<AMHGAGameState>();
	if (!gs || gs->bIsGameOver) return;

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

void AMHGAGameMode::HandleGameOver(FString reason)
{
	AMHGAGameState* gs = GetGameState<AMHGAGameState>();
	if (gs && !gs->bIsGameOver)
	{
		gs->bIsGameOver = true;
		UE_LOG(LogTemp, Error, TEXT("서버 : 영업 종료! 사유 : %s"), *reason);
		// TODO : 게임 종료 처리(플레이어 입력 중지, 결과화면 표시)
	}
}
