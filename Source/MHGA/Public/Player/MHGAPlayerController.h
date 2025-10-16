// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MHGAPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class MHGA_API AMHGAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMHGAPlayerController();

protected:
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	UInputMappingContext* IMC;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;
};
