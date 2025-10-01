#pragma once

#include "CoreMinimal.h"
#include "BurgerData.generated.h"

UENUM(BlueprintType)
enum class EIngredient : uint8
{
	BottomBread		UMETA(DisplayName = "BottomBread"),
	MiddleBread		UMETA(DisplayName = "MiddleBread"),
	TopBread		UMETA(DisplayName = "TopBread"),
	Patty			UMETA(DisplayName = "Patty"),
	Lettuce			UMETA(DisplayName = "Lettuce"),
	Tomato			UMETA(DisplayName = "Tomato"),
	Onion			UMETA(DisplayName = "Onion"),
	Bacon			UMETA(DisplayName = "Bacon"),
	Cheese			UMETA(DisplayName = "Cheese"),
	Pickle			UMETA(DisplayName = "Pickle"),
	Sauce			UMETA(DisplayName = "Sauce"),
	ShanghaiPortion	UMETA(DisplayName = "ShanghaiPotion"),
	ShrimpPortion	UMETA(DisplayName = "ShrimpPotion"),
};

UENUM(BlueprintType)
enum class EBurgerMenu : uint8
{
	BigMac				UMETA(DisplayName = "BigMac"),
	BTD					UMETA(DisplayName = "BTD"),
	QPC					UMETA(DisplayName = "QPC"),
	Shanghai			UMETA(DisplayName = "Shanghai"),
	Shrimp				UMETA(DisplayName = "Shrimp")
};

USTRUCT(BlueprintType)
struct FBurgerRecipe : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBurgerMenu BurgerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EIngredient, int32> IngredientsMap;
};
