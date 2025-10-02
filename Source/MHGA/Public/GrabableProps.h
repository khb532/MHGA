// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GrabableProps.generated.h"

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
	virtual void OnGrabbed() = 0;
};
