// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MHGAGameState.generated.h"

class ACounterPOS;

UCLASS()
class MHGA_API AMHGAGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMHGAGameState();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	ACounterPOS* Counter;

public:
	ACounterPOS* GetCounter() {return Counter;}

	// 평점 관련
	// 현재 평점들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateScore)
	int32 orderSpeedScore;	// 주문 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateScore)
	int32 cookSpeedScore;	// 음식 전달 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateScore)
	int32 foodScore;	// 음식 정확도

	// 평점 업데이트시 호출 함수
	UFUNCTION()
	void OnRep_UpdateScore();

	// 초기 평점
	UPROPERTY()
	int32 startScore = 100;

	// 게임 오버가 되는 평점
	UPROPERTY()
	int32 gameoverScore = 0;

	// 제한시간
	UPROPERTY(ReplicatedUsing = OnRep_UpdateTime)
	float remainTime;

	UFUNCTION()
	void OnRep_UpdateTime();

	UPROPERTY()
	float startTime = 300.0f;	// 초기 시간

	// 게임 오버 상태
	UPROPERTY(Replicated)
	bool bIsGameOver = false;
};
