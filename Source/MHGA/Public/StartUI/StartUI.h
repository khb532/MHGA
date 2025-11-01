#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartUI.generated.h"

class UEditableText;
class UJobButtonUI;
class UMHGAGameInstance;
class UEditableTextBox;
class UUniformGridPanel;
class UCanvasPanel;
class UButton;

UCLASS()
class MHGA_API UStartUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UStartUI(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
protected:
	UPROPERTY()
	UMHGAGameInstance* GI;
	UPROPERTY()
	USoundBase* ClickSound;
	
	UPROPERTY()
	TSubclassOf<UJobButtonUI> JobBtn;

	//Login
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* Canvas_Login;
	UPROPERTY(meta=(BindWidget))
	UEditableText* Input_Login;
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_Login;
	
	//Start
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* Canvas_Start;
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* Canvas_Session;
	
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_SearchJob;
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_Refresh;

	const int32 ColumnCount = 4;
	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* Grid_Panel;

	//방 만들기
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* Canvas_MakeJob;
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_MakeJob;

	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* Input_Name;
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* Input_Number;
	
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_MakeJobExit;
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_RegisterJob;


protected:
	UFUNCTION() void OnClickLoginBtn();
	UFUNCTION() void OnClickSearchBtn();
	UFUNCTION() void OnClickRefreshBtn();
	UFUNCTION() void OnClickMakeJobBtn();
	UFUNCTION() void OnClickMakeJobExitBtn();
	UFUNCTION() void OnClickRegisterJobBtn();

	UFUNCTION() void OnInputNumChange(const FText& Text);
	void OnFindComplete(TArray<FOnlineSessionSearchResult>& Results);
};
