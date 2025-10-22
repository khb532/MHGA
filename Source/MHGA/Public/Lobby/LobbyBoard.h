#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyBoard.generated.h"

class ULobbyUI;
class UStaticMeshComponent;
class UWidgetComponent;

UCLASS()
class MHGA_API ALobbyBoard : public AActor
{
	GENERATED_BODY()

public:
	ALobbyBoard();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(EditAnywhere)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere)
	ULobbyUI* LobbyUI;

public:
	
};
