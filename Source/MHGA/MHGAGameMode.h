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
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<APawn>> characterList;

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

	void GameStart();
	void HandleGameOver(FString reason);
};