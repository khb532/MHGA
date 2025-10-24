#include "WrappingPaper.h"
#include "Hamburger.h"
#include "MHGA.h"
#include "Components/BoxComponent.h"
#include "Ingredient/IngredientBase.h"
#include "Net/UnrealNetwork.h"


AWrappingPaper::AWrappingPaper()
{
	PrimaryActorTick.bCanEverTick = true;

	// TODO(human): 이 액터가 네트워크를 통해 복제되도록 설정
	// 고려사항: 모든 클라이언트가 이 액터를 보고 상호작용해야 함
	bReplicates = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void AWrappingPaper::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWrappingPaper, OnAreaIngredients);
}

void AWrappingPaper::BeginPlay()
{
	Super::BeginPlay();
	
	Collision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AWrappingPaper::AddIngredient);

	Collision->OnComponentEndOverlap.AddDynamic(this, &AWrappingPaper::RemoveIngredient);
	
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

void AWrappingPaper::AddIngredient_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// TODO(human): 이 함수가 서버에서만 실행되도록 보장
	// 고려사항: 클라이언트에서 실행되면 재료가 중복 추가될 수 있음
	if (!HasAuthority()) return;
	
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

void AWrappingPaper::RemoveIngredient_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// TODO(human): 이 함수도 서버에서만 실행되도록 보장
	// 고려사항: AddIngredient와 동일한 패턴 적용
	if (!HasAuthority()) return;

	// OFF Overlap
	if (bShowLog)
		GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::Red, "Off Overlapped & MinusIngredient");

	 // 1. Collide Out, if(self) return
	if (OtherActor == nullptr || OtherActor == this) return;

	for (int32 i = OverlappingActors.Num() - 1; i >= 0; --i)
	{
		if (OverlappingActors[i].Get() == OtherActor)
		{
			OverlappingActors.RemoveAt(i);
		}
	}
	
	bool hasNotFound = true;
	AIngredientBase* Ingredient = Cast<AIngredientBase>(OtherActor);
	
	if (Ingredient == nullptr) 
	{
		for (int32 i = 0 ; i < OnAreaIngredients.Num(); i++)
		{
			if (OnAreaIngredients[i].IngredientId == EIngredient::None)
			{
				hasNotFound = false;
				--OnAreaIngredients[i].Quantity;
				if (OnAreaIngredients[i].Quantity <= 0)
					OnAreaIngredients.RemoveAt(i);
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
				hasNotFound = false;
				// 3. Found. Quantity - 1
				// 4. Quantity <= 0, RemoveAt
				--OnAreaIngredients[i].Quantity;
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

void AWrappingPaper::TryWrap_Implementation()
{
	// TODO(human): TryWrap을 서버 RPC로 변경
	// 고려사항: 클라이언트가 포장 버튼을 누르면 서버에 요청, 서버가 실제 포장 처리
	if (HasAuthority())
	{
		if (HasBreadPair() && HasExtraIngredient())
		{
			PRINTLOG(TEXT("TryWrap Not Problem"))
			CompleteWrapping();
		}
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

void AWrappingPaper::OnRep_AddIng()
{
	
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

bool AWrappingPaper::HasBreadPair() const
{
	// OnAreaIngredients 배열을 순회하며 BottomBread와 TopBread 수량을 각각 추적한다.
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
	
	// 두 빵의 수량이 모두 1 이상이면 true를 반환하고, 그렇지 않으면 false를 반환한다.
	if (Q_TBread >= 1 && Q_BBread >= 1)
	{
		PRINTLOG(TEXT("Bread is TRUE"))
	}
	else
	{
		PRINTLOG(TEXT("Bread is FALSE"))
	}
	return Q_TBread >= 1 && Q_BBread >= 1;
}

bool AWrappingPaper::HasExtraIngredient() const
{
	for (const FIngredientStack& tmp : OnAreaIngredients)
	{
		if (!(tmp.IngredientId == EIngredient::BottomBread || tmp.IngredientId == EIngredient::TopBread))
		{
			PRINTLOG(TEXT("Extra is TRUE"))
			return true;
		}
	}
	PRINTLOG(TEXT("Extra is FALSE"))
	return false;
}

void AWrappingPaper::CompleteWrapping()
{
	// TODO(human): 이 함수가 서버에서만 실행되도록 보장
	// 고려사항: 햄버거 스폰은 서버만 수행, 클라이언트는 자동으로 복제된 햄버거를 봄
	if (!HasAuthority()) return;
	PRINTLOG(TEXT("CompleteWrap Not Problem"))
	// BurgerDataTable과 CompletedBurgerClass가 설정되어 있는지 확인한다.
	if (!(BurgerDataTable && BurgerClass)) return;
	// FindMatchingRecipe를 호출해 현재 재료 조합에 대응되는 EBurgerMenu를 얻는다.
	EBurgerMenu CreatedBurgerName = FindMatchingRecipe(BurgerDataTable, OnAreaIngredients);
	const UEnum* BurgerEnum = StaticEnum<EBurgerMenu>();
	FString Str = BurgerEnum->GetNameStringByValue(static_cast<int64>(CreatedBurgerName));

	if (bShowLog)
		PRINTLOG(TEXT("%s"), *Str);
	
	DestroyIngredients();
	
	// CompletedBurgerClass를 SpawnActor하여 햄버거 액터를 생성한다.
	AHamburger* SpawnedBurger = GetWorld()->SpawnActor<AHamburger>(BurgerClass, this->GetActorTransform());
	// SpawnedBurger->SetMenu()
	SpawnedBurger->SetName(Str);
	// DestroyIngredients()를 호출해 재료와 포장지를 정리한다.
}

void AWrappingPaper::DestroyIngredients()
{
	// OverlappingActors에 저장된 액터 포인터를 순회하며 Destroy를 호출한다.
	TArray<TWeakObjectPtr<AActor>> ActorsToDestroy = OverlappingActors;
	// OnAreaIngredients와 OverlappingActors를 비워 다음 사용에 대비한다.
	OverlappingActors.Empty();
	OnAreaIngredients.Empty();

	for (const TWeakObjectPtr<AActor>& Ptr : ActorsToDestroy)
	{
		if (AActor* Actor = Ptr.Get())
		{
			Actor->Destroy();
		}
	}
	// WrappingPaper 자신도 Destroy하여 포장 과정을 마무리한다.
	Destroy();
}
