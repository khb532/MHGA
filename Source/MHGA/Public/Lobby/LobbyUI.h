#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUI.generated.h"

class UVerticalBox;
class UTextBlock;
class ALobbyBoard;
class UButton;

UCLASS()
class MHGA_API ULobbyUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	ALobbyBoard* LobbyBoard;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VB_Player;
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VB_Ready;

	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Ready;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TEXT_Ready;
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Run;

	UPROPERTY()
	TArray<UTextBlock*> PlayerArr;
	UPROPERTY()
	TArray<UTextBlock*> ReadyArr;

	int32 PlayerCount;
	TArray<bool> IsPlayerReady;

public:
	void Init(ALobbyBoard* InLobbyBoard);
	UFUNCTION() void OnClickReady();
	UFUNCTION() void OnClickRun();
	
	UFUNCTION() void Ready(int32 PlayerNum);
	UFUNCTION() void Run();
	UFUNCTION() void Refresh(int32 PlayerNum);
};
