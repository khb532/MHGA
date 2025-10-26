// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GrabableProps.generated.h"

class AMHGACharacter;
// This class does not need to be modified.
UINTERFACE()
class UGrabableProps : public UInterface
{
	GENERATED_BODY()
};

class MHGA_API IGrabableProps
{
	GENERATED_BODY()

public:
	UFUNCTION(Category="GrabComp")
	virtual void OnGrabbed(AMHGACharacter* Player) = 0;
	UFUNCTION(Category="GrabComp")
	virtual void OnPut() = 0;
	UFUNCTION(Category="GrabComp")
	virtual void OnUse() = 0;
	UFUNCTION(Category="GrabComp")
	virtual void SetLocation(FVector Loc) = 0;

	UFUNCTION(Category="GrabComp")
	virtual UStaticMeshComponent* GetMeshComp() = 0;
};
