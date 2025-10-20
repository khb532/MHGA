#pragma once

#include "CoreMinimal.h"
#include "IngredientBase.h"
#include "Patty.generated.h"

// 패티의 요리 상태
UENUM(BlueprintType)
enum class EPattyCookState : uint8
{
	Raw,
	Cooked,
	Overcooked
};

UCLASS()
class MHGA_API APatty : public AIngredientBase
{
	GENERATED_BODY()

public:
	APatty();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 외부(CookingArea)에서 이 패티가 구워졌는지 확인하는 함수
	UFUNCTION(BlueprintCallable, Category = "Patty")
	EPattyCookState GetCookState() const;

	// 패티의 상태를 변경하는 함수
	void SetCookState(EPattyCookState NewState);

private:
	// 패티의 구워짐 상태
	EPattyCookState CookState = EPattyCookState::Raw;
};
