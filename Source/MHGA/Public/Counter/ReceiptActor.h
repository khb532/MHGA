// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabableProps.h"
#include "GameFramework/Actor.h"
#include "ReceiptActor.generated.h"

class UBoxComponent;
class UCanvas;
class UCanvasRenderTarget2D;
class UMaterialInstanceDynamic;
class UStaticMeshComponent;
UCLASS()
class MHGA_API AReceiptActor : public AActor, public IGrabableProps
{
	GENERATED_BODY()
	
public:
	AReceiptActor();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PaperMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* BaseMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMat;

	UPROPERTY()
	UCanvasRenderTarget2D* RenderTarget;

	int32 OrderNum;
	TArray<FString> MenuList;

protected:
	UFUNCTION()
	void OnUpdateCanvas(UCanvas* Canvas, int32 Width, int32 Height);

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Init(int32 InOrderNum, const TArray<FString>& InMenus);

	virtual void OnGrabbed(AMHGACharacter* Player) override {}
	virtual void OnPut() override {}
	virtual void OnUse() override{}
	virtual void SetLocation(FVector Loc) override;
};