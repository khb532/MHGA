// Fill out your copyright notice in the Description page of Project Settings.


#include "MHGAGameState.h"

#include "PlayerWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Counter/CounterPOS.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/MHGAPlayerController.h"

AMHGAGameState::AMHGAGameState()
{
	bReplicates = true;
	
	orderSpeedScore = startScore;
	cookSpeedScore = startScore;
	foodScore = startScore;
	remainTime = startTime;
	bIsGameOver = false;
}

void AMHGAGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
		Counter = Cast<ACounterPOS>(UGameplayStatics::GetActorOfClass(GetWorld(), ACounterPOS::StaticClass()));
}

void AMHGAGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMHGAGameState, Counter);
	
	DOREPLIFETIME(AMHGAGameState, orderSpeedScore);
	DOREPLIFETIME(AMHGAGameState, cookSpeedScore);
	DOREPLIFETIME(AMHGAGameState, foodScore);
	DOREPLIFETIME(AMHGAGameState, finalScore);
	DOREPLIFETIME(AMHGAGameState, remainTime);
	DOREPLIFETIME(AMHGAGameState, bIsGamePlaying);
	DOREPLIFETIME(AMHGAGameState, bIsGameOver);
}

void AMHGAGameState::OnRep_UpdateScore()
{
	// TODO : HUD 평점 표시 업데이트 로직
	// 예: APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0); if(MyPawn) MyPawn->UpdateReputationUI(CurrentReputation);
	UE_LOG(LogTemp, Log, TEXT("클라이언트 : 평점 업테이트됨 (주문속도 : %d, 조리속도 : %d, 정확도 : %d"), orderSpeedScore, cookSpeedScore, foodScore)

	// 게임 오버 조건 확인
}

void AMHGAGameState::OnRep_UpdateTime()
{
	UE_LOG(LogTemp, Log, TEXT("클라이언트 : 남은시간 %.1f초"), remainTime);
	// TODO: GetOwningPlayerController()->GetHUD()->UpdateTimeUI(RemainingTime); 호출
}

void AMHGAGameState::OnRep_GameStart()
{
	if (bIsGamePlaying)
	{
		playerWidget->startCanvas->SetVisibility(ESlateVisibility::Visible);
		playerWidget->PlayStartAnim();
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			playerWidget->startCanvas->SetVisibility(ESlateVisibility::Hidden);
		}, 3, false);
	}
}

FText AMHGAGameState::GetFinalRank()
{
	if (finalScore >= 250)
	{
		return FText::FromString(TEXT("넌 정직원 해라"));
	}
	else if (finalScore >= 150)
	{
		return FText::FromString(TEXT("잘하시네요"));
	}
	else if (finalScore >= 100)
	{
		return FText::FromString(TEXT("평균입니다"));
	}
	else
	{
		return FText::FromString(TEXT("분발하세요"));
	}
}

void AMHGAGameState::OnRep_GameOver()
{
	// 이 함수는 bIsGameOver 값이 서버와 달라졌을 때 (즉, true가 되었을 때)
	// 모든 클라이언트에서 자동으로 호출됩니다.
	if (bIsGameOver)
	{
		UE_LOG(LogTemp, Warning, TEXT("클라이언트: OnRep_GameOver 수신. 게임 오버 처리 시작."));

		playerWidget->finalScore->SetVisibility(ESlateVisibility::Visible);
		playerWidget->PlayFinalScoreAnim();
		
		// 이 클라이언트의 로컬 플레이어 컨트롤러를 가져옵니다.
		AMHGAPlayerController* pc = Cast<AMHGAPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		// TODO: AYourPlayerController를 본인의 플레이어 컨트롤러 클래스로 캐스팅하세요.
		if (pc)
		{
			// 플레이어 컨트롤러에 실제 게임 오버 처리를 지시합니다.
			// 점수도 함께 넘겨줍니다.
			pc->Client_HandleGameOver();
		}
	}
}

void AMHGAGameState::InitUI(UPlayerWidget* widget)
{
	playerWidget = widget;
}

