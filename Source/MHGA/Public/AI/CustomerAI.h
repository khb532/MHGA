// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueData.h"
#include "GameFramework/Character.h"
#include "CustomerAI.generated.h"


UCLASS()
class MHGA_API ACustomerAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACustomerAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly)
	class UCustomerFSM* fsm;
	
	// 손님 머리 위에 텍스트 UI를 표시할 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "UI")
	class UWidgetComponent* customerWidget;

	UPROPERTY(EditDefaultsOnly, Category = "AI Dialogue")
	class UDataTable* scoreDialogueTable;
	
	UFUNCTION(BlueprintCallable, Category = "AI Order")
	void ShowOrderUI();
	UFUNCTION(BlueprintCallable, Category = "AI Order")
	void HideOrderUI();

	// 대사 가져오기
	FText GetScoreDialogue(EScoreChangeReason reason);
	
	// 평점 변경 사유에 따른 대사 표시 함수
	void ShowScoreFeedback(EScoreChangeReason reason);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowScoreFeedback(EScoreChangeReason reason);

	// --- Replication Callbacks ---
	UFUNCTION()
	void OnRep_FSMStateChanged();
    
	/** 실제 위젯 인스턴스를 가져옵니다 (필요시 초기화 포함). */
	class UCustomerUI* GetCustomerUIInstance();

	

	void UpdateCustomerWidget(bool bShow, const FText& textToShow, FLinearColor textColor);
};
