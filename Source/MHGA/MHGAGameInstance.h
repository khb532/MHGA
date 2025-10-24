#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MHGAGameInstance.generated.h"

DECLARE_DELEGATE_OneParam(FFindComplete, TArray<FOnlineSessionSearchResult>&)

UCLASS()
class MHGA_API UMHGAGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	//begin play
	virtual void Init() override;

public:
	//세션 찾았을 때 해당 정보 넘기는 델리게이트
	FFindComplete FindCompleteDelegate;
	
protected:
	FString NickName = FString(TEXT("Player"));
	
	//세션 모든 처리를 진행하는 객체
	IOnlineSessionPtr SessionInterface;
	
	//현재 세션 이름
	FName CurrentSessionName;

	//세션 조회할 때 사용하는 객체
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	
	//세선 생성 완료 함수
	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);
	//세션 조회 완료 함수
	void OnFindSessionComplete(bool bWasSuccessful);
	//세션 참여 완료 함수
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	
public:
	//세션 생성 관련
	//세션 생성 함수
	UFUNCTION(BlueprintCallable)
	void CreateMySession(FString displayName, int32 playerCount);

	//세션 조회 관련
	//세션 조회 요청 함수
	UFUNCTION(BlueprintCallable)
	void FindOtherSession();

	//세션 참여 관련
	//세션 참여 함수
	UFUNCTION(BlueprintCallable)
	void JoinOtherSession(int32 sessionIdx);

	//닉네임 설정
	void SetPlayerName(FString name) {NickName = name; }
	FString GetPlayerName() {return NickName;}

	//문자열을 UTF-8 -> base64로 Encode하는 함수
	FString StringBase64Encode(FString str);
	//문자열을 base64 -> UTF-8로 Decode하는 함수
	FString StringBase64Decode(FString str);
};

