#include "WrappingPaper.h"
#include "MHGA.h"
#include "Components/BoxComponent.h"
#include "Ingredient/IngredientBase.h"
#include "Kismet/GameplayStatics.h"


AWrappingPaper::AWrappingPaper()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void AWrappingPaper::BeginPlay()
{
	Super::BeginPlay();
	
	Collision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AWrappingPaper::AddIngredient);

	Collision->OnComponentEndOverlap.AddDynamic(this, &AWrappingPaper::MinusIngredient);
	
}

void AWrappingPaper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShowLog)
		PrintLog();

	if (OnAreaIngredients.IsEmpty())
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, TEXT("No Ingredients on WrappingPaper"), false);
	else
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, TEXT("Ingredients Alive"), false);
}

void AWrappingPaper::AddIngredient(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Yellow, "On Overlapped & AddIngredient");
	FIngredientStack Prop;
	Prop.Quantity++;
	if (OtherActor == nullptr || OtherActor == this) return;
	 // if (OtherActor = 햄버거재료)
	AIngredientBase* OtherIngredient = Cast<AIngredientBase>(OtherActor);
	if (OtherIngredient == nullptr) return;
	
	Prop.IngredientId = OtherIngredient->GetIngType();
	for (FIngredientStack& tmp : OnAreaIngredients)
	{
		if (tmp.IngredientId == OtherIngredient->GetIngType())
		{
			tmp.Quantity++;
			return;
		}
	}
	OnAreaIngredients.Add(Prop);
}

void AWrappingPaper::MinusIngredient(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::Red, "Off Overlapped & MinusIngredient");
	 // * 1. CollisionBox 를 벗어난 OtherActor 가 햄버거 재료인지 판별한다. 재료가 아니거나 nullptr 이면 조기 반환한다.
	if (OtherActor == nullptr || OtherActor == this) return;
	AIngredientBase* Ingredient = Cast<AIngredientBase>(OtherActor);
	bool hasNotFound = true;
	
	if (Ingredient == nullptr) 
	{
		for (int32 i = 0 ; i < OnAreaIngredients.Num(); i++)
		{
			if (OnAreaIngredients[i].IngredientId == EIngredient::None)
			{
				--OnAreaIngredients[i].Quantity;
				if (OnAreaIngredients[i].Quantity <= 0)
					OnAreaIngredients.RemoveAt(i);
				hasNotFound = false;
				break;
			}
		}
	}
	 // * 2. AddIngredient 에서 OnAreaIngredients 배열에 저장해둔 FIngredientStack 중에서 OtherActor 와 매칭되는 항목(IngredientId 기준 혹은 Actor 포인터 매핑 기준)을 찾는다.
	else
	{
	// * 3. 찾은 항목의 Quantity 를 1 감소시킨다.
		for (int32 i = 0 ; i < OnAreaIngredients.Num(); i++)
		{
			if (OnAreaIngredients[i].IngredientId == Ingredient->GetIngType())
			{
	// * 4. 감소된 Quantity 가 0 이하라면 RemoveAt 또는 RemoveSingleSwap 으로 해당 항목을 배열에서 제거한다.
				--OnAreaIngredients[i].Quantity;
				hasNotFound = false;
				if (OnAreaIngredients[i].Quantity <= 0)
					OnAreaIngredients.RemoveAt(i);
				break;
			}
		}
	}
	// * 5. 배열에 해당 항목이 없을 경우 대비하여 예외 처리를 하고, 필요하면 로그로 남겨 디버깅한다.
	if (hasNotFound)
	{
		PRINTLOG(TEXT("No Ing in Array"));
	}

	
}

void AWrappingPaper::PrintLog()
{
	
	FString ActorName = this->GetActorNameOrLabel();
	const UEnum* IngEnum = StaticEnum<EIngredient>();
	if (IngEnum == nullptr) return;
	if (OnAreaIngredients.Num() <= 0) return;
	FString Str = FString::Printf(TEXT("%s\n"), *ActorName);
	
	for (int32 i = 0; i < OnAreaIngredients.Num(); i++)
	{
		FString ing = IngEnum->GetNameStringByValue(static_cast<int64>(OnAreaIngredients[i].IngredientId));
		Str += FString::Printf(TEXT("%d 번째 재료 : %s , 수량 : %d\n"), i+1, *ing, OnAreaIngredients[i].Quantity);
	}
	
	DrawDebugString(GetWorld(), GetActorLocation(), Str, nullptr, FColor::Yellow, 0);
}

TMap<EIngredient, int32> AWrappingPaper::MakeMapFromArray(const TArray<FIngredientStack>& InArray)
{
	TMap<EIngredient, int32> Result;

	for (const FIngredientStack& it : InArray)
	{
		// InArray의 재료이름 별로 <Map>Result 구성
		Result.FindOrAdd(it.IngredientId) += it.Quantity;
	}
	
	return Result;
}

EBurgerMenu AWrappingPaper::FindMatchingRecipe(UDataTable* DT, const TArray<FIngredientStack>& WrapperIngr)
{
	TMap<EIngredient, int32> WrapMap = MakeMapFromArray(WrapperIngr);
	TArray<FBurgerRecipe*> AllRows;
	// DT 불러오기
	DT->GetAllRows<FBurgerRecipe>(TEXT("FBurgerRecipe"), AllRows);
	// DT의 행마다 반복
	for (FBurgerRecipe* Row : AllRows)
	{
		if (!Row) continue;
		TMap<EIngredient, int32> RecipeMap = MakeMapFromArray(Row->Ingredients);
		// 현재 행의 TMap 키 수 != WrapperIngr의 키 수 : 재료수부터 다름 return;
		if (RecipeMap.Num() != WrapMap.Num()) continue;

		bool isMatched = true;
		// 현재 행의 Map Key->WrapperIngr의 Map key 조회 후 Quantity와 비교
		for (const auto& Pair : RecipeMap)
		{
			const int32* WrapQty = WrapMap.Find(Pair.Key);
			if (WrapQty == nullptr || *WrapQty != Pair.Value)
			{
				isMatched = false;
				break;
			}
		}

		if (isMatched)
			return Row->BurgerName;
		
	}

	
	return EBurgerMenu::WrongBurger;
}


