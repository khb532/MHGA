#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LoadingBurger.generated.h"

UCLASS()
class MHGA_API ALoadingBurger : public AActor
{
	GENERATED_BODY()

public:
	ALoadingBurger();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Scene Capture Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneCaptureComponent2D> Capture;

	// 햄버거 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 회전 속도
	UPROPERTY(EditAnywhere, Category = "Loading")
	float RotationSpeed = 90.0f; // 초당 90도

	// Render Target
	UPROPERTY(EditAnywhere, Category = "Loading")
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

};
