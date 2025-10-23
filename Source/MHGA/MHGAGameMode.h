#pragma once

#include "CoreMinimal.h"
#include "DialogueData.h"
#include "MHGAGameState.h"
#include "GameFramework/GameModeBase.h"
#include "MHGAGameMode.generated.h"

UCLASS()
class AMHGAGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMHGAGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 평점 관리
	void ReportScoreChanged(EScoreChangeReason reason, int32 changeScore);

	// --- 평점 설정 ---
	UPROPERTY(EditAnywhere, Category = "Game Config")
	int32 penaltySlowOrder = -5;
	UPROPERTY(EditAnywhere, Category = "Game Config")
	int32 penaltySlowCook = -10;
	UPROPERTY(EditAnywhere, Category = "Game Config")
	int32 penaltyWrongFood = -15;
	UPROPERTY(EditAnywhere, Category = "Game Config")
	int32 bonusCorrectFood = 10;

	void HandleGameOver(FString reason);
};