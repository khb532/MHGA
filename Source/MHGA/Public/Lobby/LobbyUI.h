#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUI.generated.h"

class UBoardText;
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

	//버티컬 박스
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VB_Player;
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VB_Ready;

	//레디, 도망 버튼
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Ready;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TEXT_Ready;
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Run;

	//플레이어 텍스트 어레이
	UPROPERTY()
	TArray<UBoardText*> PlayerArr;
	UPROPERTY()
	TArray<UBoardText*> ReadyArr;
	
	//생성할 보드 텍스트
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBoardText> BoardText;


public:
	void Init(ALobbyBoard* InLobbyBoard);
	UFUNCTION() void OnClickReady();
	UFUNCTION() void OnClickRun();
	
	UFUNCTION() void Ready(int32 PlayerNum);
	UFUNCTION() void Run();
	UFUNCTION() void Refresh(TArray<FString>& Names);
};
