// Fill out your copyright notice in the Description page of Project Settings.


#include "MHGAGameState.h"

#include "Counter/CounterPOS.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
	DOREPLIFETIME(AMHGAGameState, remainTime);
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
