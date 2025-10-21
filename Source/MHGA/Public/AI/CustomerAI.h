// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCustomerFSM* fsm;
	
	// 손님 머리 위에 텍스트 UI를 표시할 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetComponent* widget;
	
	// FSM의 데이터를 대신 전달해주는 '대리인' 함수
	UFUNCTION(BlueprintPure, Category = "AI Order")
	FText GetOrderTextFromFSM();
	
	// 에디터에서 지정할 위젯 블루프린트
	UPROPERTY(EditAnywhere, Category = "AI Order")
	TSubclassOf<UUserWidget> orderWidget;
	// 생성된 위젯의 인스턴스를 저장할 변수
	UPROPERTY(EditAnywhere, Category = "AI Order")
	TObjectPtr<UUserWidget> orderWidgetInst;
	// UI 표시하기 / 숨기기
	UFUNCTION(BlueprintCallable, Category = "AI Order")
	void ShowOrderUI();
	UFUNCTION(BlueprintCallable, Category = "AI Order")
	void HideOrderUI();
	
	// 메뉴 판단후 손님 상단 UI 표시 함수
	UFUNCTION(BlueprintCallable, Category = "AI Order")
	void ShowReputationText(bool bIsPositive);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowReputationText(bool bIsPositive);
	
};
