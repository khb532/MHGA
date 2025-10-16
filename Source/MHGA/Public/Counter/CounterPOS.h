// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BurgerData.h"
#include "CounterPOS.generated.h"

class UCounterUI;
class UWidgetComponent;

USTRUCT(BlueprintType)
struct FOrderArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EBurgerMenu> Menus;
};

UCLASS()
class MHGA_API ACounterPOS : public AActor
{
	GENERATED_BODY()

public:
	ACounterPOS();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(EditAnywhere)
	UWidgetComponent* WidgetComponent;
	
	UPROPERTY(EditAnywhere)
	UCounterUI* CounterUI;

	//UPROPERTY(EditAnywhere)
	//AActor* CurrentCustomer;

public:
	UPROPERTY(EditAnywhere)
	int32 OrderNum = 100;
	UPROPERTY(EditAnywhere)
	TMap<int32, FOrderArray> OrderMap;

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_OnClickCustomerBtn();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_OnClickMenuBtn();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_OrderMenuBtn();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_DeleteListBtn();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_OnMenuReadyBtn();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnClickCustomerBtn();
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnClickMenuBtn();
	UFUNCTION(Server, Reliable)
	void ServerRPC_OrderMenuBtn();
	UFUNCTION(Server, Reliable)
	void ServerRPC_DeleteListBtn();
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnMenuReadyBtn();
	
	//TODO : 손님 get, set


	void PrintNetLog();
};
