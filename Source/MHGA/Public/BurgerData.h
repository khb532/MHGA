#pragma once

#include "CoreMinimal.h"
#include "BurgerData.generated.h"

UENUM(BlueprintType)
enum class EIngredient : uint8
{
	BottomBread		UMETA(DisplayName = "BottomBread"),
	Patty			UMETA(DisplayName = "Patty"),
	TopBread		UMETA(DisplayName = "TopBread")
};

UENUM(BlueprintType)
enum class EBurgerMenu : uint8
{
	Bigmac				UMETA(DisplayName = "Bigmac"),
	Boolgogi			UMETA(DisplayName = "Boolgogi"),
	Cheeseburger		UMETA(DisplayName = "Cheeseburger")
};

USTRUCT(BlueprintType)
struct FBurgerRecipe : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBurgerMenu BurgerName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EIngredient> Ingredients;
};
