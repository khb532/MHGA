// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabableProps.h"
#include "GameFramework/Actor.h"
#include "Flipper.generated.h"

UCLASS()
class MHGA_API AFlipper : public AActor, public IGrabableProps
{
	GENERATED_BODY()

public:
	AFlipper();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Props")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Props")
	AMHGACharacter* GrabCharacter;
	
public:

	virtual void OnGrabbed(AMHGACharacter* Player) override;
	virtual void OnPut() override;
	virtual void OnUse() override;
	virtual void SetLocation(FVector Loc) override;
};
