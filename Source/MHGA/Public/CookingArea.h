#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CookingArea.generated.h"

UCLASS()
class MHGA_API ACookingArea : public AActor
{
	GENERATED_BODY()
	
public:	
	ACookingArea();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* boxComp;

	// 요리되는 시간
	UPROPERTY(EditAnywhere, Category = "Settings")
	float cookingTime = 3.0f;
	// 오버쿡까지 걸리는 추가 시간
	UPROPERTY(EditAnywhere, Category = "Settings")
	float OvercookTime = 5.0f;
	// 변경할 파라미터 이름
	UPROPERTY(EditAnywhere, Category = "Settings")
	FName paramName = TEXT("CookingLevel");
	// 요리된 후 색상
	UPROPERTY(EditAnywhere, Category = "Settings")
	FLinearColor cookedColor = FLinearColor::Red;
	// 오버쿡 상태일 때 색상
	UPROPERTY(EditAnywhere, Category = "Settings")
	FLinearColor overcookedColor = FLinearColor::Black;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/////* ChanageColor => Patty:UpdateMaterial로 대체됨 (khb) */////
	// 색 변경 함수
	// void ChangeColor(AActor* actor, FLinearColor newColor);

	UFUNCTION()
	void CookPatty(AActor* actor);

	UFUNCTION()
	void OvercookPatty(AActor* actor);

	// TMap을 사용하여 여러 액터와 그에 해당하는 타이머 핸들을 저장합니다.
	// Key: 오버랩된 액터의 포인터
	// Value: 해당 액터를 위한 타이머 핸들
	TMap<AActor*, FTimerHandle> overlapActorTimer;
};
