#include "WrappingPaper.h"
#include "Hamburger.h"
#include "MHGA.h"
#include "Components/BoxComponent.h"
#include "Ingredient/IngredientBase.h"


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
	{
		PrintLog();
		
		if (OnAreaIngredients.IsEmpty())
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, TEXT("No Ingredients on WrappingPaper"), false);
		else
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, TEXT("Ingredients Alive"), false);
	}
}

void AWrappingPaper::AddIngredient(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// On Overlap
	if (bShowLog)
		GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Yellow, "On Overlapped & AddIngredient");
	
	if (OtherActor == nullptr || OtherActor == this) return;

	OverlappingActors.AddUnique(OtherActor);

	FIngredientStack Prop;
	Prop.Quantity++;
	// if (OtherActor == Ingredient)
	AIngredientBase* OtherIngredient = Cast<AIngredientBase>(OtherActor);
	if (OtherIngredient != nullptr) 
	{
		const EIngredient IngId = OtherIngredient->GetIngType();
		Prop.IngredientId = IngId;
		
		for (FIngredientStack& tmp : OnAreaIngredients)
		{
			if (tmp.IngredientId == IngId)
			{
				tmp.Quantity++;
				return;
			}
		}
		OnAreaIngredients.Add(Prop);
	}
	else
	{
		Prop.IngredientId = EIngredient::None;
		FIngredientStack* FoundStack = OnAreaIngredients.FindByPredicate(
			[ingid = Prop.IngredientId](const FIngredientStack& Stack)
			{
				return Stack.IngredientId == ingid;
			});
		if (FoundStack != nullptr)
		{
			FoundStack->Quantity++;
		}
		else
		{
			OnAreaIngredients.Add(Prop);
		}
	}
}

void AWrappingPaper::MinusIngredient(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// OFF Overlap
	if (bShowLog)
		GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::Red, "Off Overlapped & MinusIngredient");
	
	 // 1. Collide Out, if(Not Ingredient OR self) return
	if (OtherActor == nullptr || OtherActor == this) return;

	for (int32 i = OverlappingActors.Num() - 1; i >= 0; --i)
	{
		if (OverlappingActors[i].Get() == OtherActor)
		{
			OverlappingActors.RemoveAt(i);
		}
	}

	AIngredientBase* Ingredient = Cast<AIngredientBase>(OtherActor);
	if (Ingredient == nullptr) return;

	bool hasNotFound = true;
	
	if (Ingredient->GetIngType() == EIngredient::None) 
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
	 // 2. Find OnAreaIngredients Matches
	else
	{
		for (int32 i = 0 ; i < OnAreaIngredients.Num(); i++)
		{
			if (OnAreaIngredients[i].IngredientId == Ingredient->GetIngType())
			{
				// 3. Found. Quantity - 1
				--OnAreaIngredients[i].Quantity;
				hasNotFound = false;
				// 4. Quantity <= 0, RemoveAt
				if (OnAreaIngredients[i].Quantity <= 0)
					OnAreaIngredients.RemoveAt(i);
				break;
			}
		}
	}
	// 5. NotFound Exception
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
	// Load Table
	DT->GetAllRows<FBurgerRecipe>(TEXT("FBurgerRecipe"), AllRows);
	// Loop Table Row
	for (FBurgerRecipe* Row : AllRows)
	{
		if (!Row) continue;
		TMap<EIngredient, int32> RecipeMap = MakeMapFromArray(Row->Ingredients);
		// Curr Row, TMap 키 수 != WrapperIngr의 키 수 : 재료수부터 다름 return;
		if (RecipeMap.Num() != WrapMap.Num()) continue;

		bool isMatched = true;
		// Curr Row, Map Key->WrapperIngr의 Map key 조회 후 Quantity와 비교
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

void AWrappingPaper::TryWrap()
{
	// TODO: AddIngredient 처리 직후 호출된다; 방금 추가된 재료가 빵일 때만 호출되도록 AddIngredient에서 제어한다.
	// TODO: HasBreadPair()로 Top/Bottom Bread가 모두 존재하는지 확인한다.
	// TODO: HasExtraIngredient()로 빵 이외 재료가 있는지 점검한다.
	// TODO: 두 조건이 모두 true일 때 CompleteWrapping()을 호출한다.
	if (HasBreadPair() && HasExtraIngredient())
		CompleteWrapping();
}

bool AWrappingPaper::HasBreadPair() const
{
	// TODO: OnAreaIngredients 배열을 순회하며 BottomBread와 TopBread 수량을 각각 추적한다.
	int32 Q_TBread = 0, Q_BBread = 0;

	for (const FIngredientStack& tmp : OnAreaIngredients)
	{
		if (tmp.IngredientId == EIngredient::TopBread)
		{
			Q_TBread += tmp.Quantity;	
		}
		else if (tmp.IngredientId == EIngredient::BottomBread)
		{
			Q_BBread += tmp.Quantity;
		}
	}
	
	// TODO: 두 빵의 수량이 모두 1 이상이면 true를 반환하고, 그렇지 않으면 false를 반환한다.
	return Q_TBread >= 1 && Q_BBread >= 1;
}

bool AWrappingPaper::HasExtraIngredient() const
{
	// TODO: OnAreaIngredients 배열에서 빵 이외 재료들의 총 수량을 누적한다.
	int32 count = 0;
	for (const FIngredientStack& tmp : OnAreaIngredients)
	{
		if (!(tmp.IngredientId == EIngredient::BottomBread || tmp.IngredientId == EIngredient::TopBread))
			count++;
	}
	// TODO: 누적된 수량이 1 이상이면 true, 그렇지 않으면 false를 반환한다.
	return count > 0 ? true : false;
}

void AWrappingPaper::CompleteWrapping()
{
	// TODO: BurgerDataTable과 CompletedBurgerClass가 설정되어 있는지 확인한다.
	if (!(BurgerDataTable && BurgerClass)) return;
	// TODO: FindMatchingRecipe를 호출해 현재 재료 조합에 대응되는 EBurgerMenu를 얻는다.
	EBurgerMenu CreatedBurgerName = FindMatchingRecipe(BurgerDataTable, OnAreaIngredients);
	const UEnum* BurgerEnum = StaticEnum<EBurgerMenu>();
	FString Str = BurgerEnum->GetNameStringByValue(static_cast<int64>(CreatedBurgerName));

	if (bShowLog)
		PRINTLOG(TEXT("%s"), *Str);
	
	DestroyIngredients();
	
	// TODO: CompletedBurgerClass를 SpawnActor하여 햄버거 액터를 생성한다.
	AHamburger* SpawnedBurger = GetWorld()->SpawnActor<AHamburger>(BurgerClass, this->GetActorTransform());
	// TODO: SpawnedBurger->SetMenu()
	SpawnedBurger->SetName(Str);
	// TODO: DestroyIngredients()를 호출해 재료와 포장지를 정리한다.
}

void AWrappingPaper::DestroyIngredients()
{
	// TODO: OverlappingActors에 저장된 액터 포인터를 순회하며 Destroy를 호출한다.
	TArray<TWeakObjectPtr<AActor>> ActorsToDestroy = OverlappingActors;
	// TODO: OnAreaIngredients와 OverlappingActors를 비워 다음 사용에 대비한다.
	OverlappingActors.Empty();
	OnAreaIngredients.Empty();

	for (const TWeakObjectPtr<AActor>& Ptr : ActorsToDestroy)
	{
		if (AActor* Actor = Ptr.Get())
		{
			Actor->Destroy();
		}
	}
	// TODO: WrappingPaper 자신도 Destroy하여 포장 과정을 마무리한다.
	Destroy();
}
