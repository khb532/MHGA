# WrappingPaper 시스템 설계 문서

## 개요

**작성자**: KHB (본인 구현)
**구현 범위**: WrappingPaper, IngredientBase, BurgerData, Hamburger

이 문서는 **내가 직접 구현한 햄버거 조립 시스템**의 설계와 구현을 설명합니다. `AWrappingPaper`는 게임의 핵심 메커니즘으로, 플레이어가 재료를 모아 햄버거를 만드는 전체 프로세스를 담당합니다.

**내가 구현한 핵심 기능**:
- ✅ 재료 오버랩 감지 및 수량 추적 (`AddIngredient`, `RemoveIngredient`)
- ✅ DataTable 기반 레시피 매칭 알고리즘 (`FindMatchingRecipe`)
- ✅ 햄버거 스폰 및 재료 자동 정리 (`CompleteWrapping`, `DestroyIngredients`)
- ✅ 멀티플레이 네트워크 동기화 (Server RPC + Replication)
- ✅ 재료 베이스 클래스 (AIngredientBase + 25종 서브클래스)
- ✅ 햄버거 데이터 정의 (BurgerData.h - Enum 및 구조체)

**외부 의존성** (팀원 구현):
- InteractComponent (Player팀 - 집기/놓기 시스템)
- MHGACharacter (Player팀 - 캐릭터 컨트롤러)
- IGrabableProps (Player팀 - 상호작용 인터페이스)

---

## 클래스 다이어그램 (내 구현 범위)

```mermaid
classDiagram
    %% 내가 구현한 클래스들 (녹색 배경)
    class AWrappingPaper {
        <<본인 구현>>
        +UBoxComponent* Collision
        +UStaticMeshComponent* Mesh
        +UDataTable* BurgerDataTable
        +TSubclassOf~AHamburger~ BurgerClass
        -TArray~FIngredientStack~ OnAreaIngredients
        -TArray~TWeakObjectPtr~AActor~~ OverlappingActors

        +AddIngredient() [Server RPC]
        +RemoveIngredient() [Server RPC]
        +TryWrap() [Server RPC]
        +CompleteWrapping()
        -FindMatchingRecipe()
        -HasBreadPair()
        -HasExtraIngredient()
        -DestroyIngredients()
    }

    class AHamburger {
        <<본인 구현>>
        +UStaticMeshComponent* Mesh
        -FString BurgerName [Replicated]
        +ServerSetName() [Server RPC]
        +SetName()
        +GetBurgerName()
    }

    class AIngredientBase {
        <<본인 구현>>
        #UStaticMeshComponent* Mesh
        #EIngredient IngType [Replicated]
        +OnGrabbed()
        +OnPut()
        +OnUse()
        +GetIngType()
    }

    class FBurgerRecipe {
        <<본인 구현 - struct>>
        +EBurgerMenu BurgerName
        +TArray~FIngredientStack~ Ingredients
    }

    class FIngredientStack {
        <<본인 구현 - struct>>
        +EIngredient IngredientId
        +int32 Quantity
    }

    class EIngredient {
        <<본인 구현 - enum>>
        None, BottomBread, TopBread, MiddleBread
        RawPatty, WellDonePatty, OvercookedPatty
        Lettuce, Tomato, Onion, Bacon, Cheese, Pickle
        Sauce, ShanghaiPortion, ShrimpPortion
    }

    class EBurgerMenu {
        <<본인 구현 - enum>>
        None, WrongBurger, BigMac
        BTD, QPC, Shanghai, Shrimp
    }

    %% 외부 인터페이스 (팀원 구현 - 회색 배경)
    class IGrabableProps {
        <<외부 인터페이스 - Player팀>>
        +OnGrabbed()*
        +OnPut()*
        +OnUse()*
        +SetLocation()*
    }

    %% 내 구현과의 관계
    AWrappingPaper --> AHamburger : spawns (내가 구현)
    AWrappingPaper --> AIngredientBase : detects overlap
    AWrappingPaper --> FBurgerRecipe : uses DataTable
    AWrappingPaper --> FIngredientStack : stores
    FBurgerRecipe --> FIngredientStack : contains
    FIngredientStack --> EIngredient : references
    FBurgerRecipe --> EBurgerMenu : defines

    %% 외부 시스템과의 인터페이스
    AIngredientBase ..|> IGrabableProps : implements (Player팀 인터페이스)
    AHamburger ..|> IGrabableProps : implements (Player팀 인터페이스)

    style AWrappingPaper fill:#ccffcc
    style AHamburger fill:#ccffcc
    style AIngredientBase fill:#ccffcc
    style FBurgerRecipe fill:#ccffcc
    style FIngredientStack fill:#ccffcc
    style EIngredient fill:#ccffcc
    style EBurgerMenu fill:#ccffcc
    style IGrabableProps fill:#dddddd
```

---

## 시퀀스 다이어그램 - 내가 구현한 WrappingPaper 동작

**참고**: InteractComponent와 MHGACharacter는 **팀원(Player팀)이 구현**한 시스템입니다.
여기서는 **내 코드(WrappingPaper, Ingredient)가 어떻게 외부 시스템과 통신하는지**만 설명합니다.

### 1. 재료 추가 플로우 (내 구현: AddIngredient)

```mermaid
sequenceDiagram
    participant External as 외부 시스템(Player팀)
    participant Ingredient as AIngredientBase(내 구현)
    participant Wrapper as AWrappingPaper(내 구현)
    participant Network as Replication(UE5)

    Note over External: 플레이어가 재료를 집어서 놓음
    External->>Ingredient: IGrabableProps 인터페이스 호출
    Ingredient->>Ingredient: OnGrabbed() 구현
    External->>Ingredient: OnPut() 구현

    Note over Ingredient,Wrapper: 재료가 WrappingPaper와 오버랩

    rect rgb(200, 255, 200)
        Note over Wrapper: 내가 구현한 AddIngredient 로직
        Ingredient->>Wrapper: OnComponentBeginOverlap
        Wrapper->>Wrapper: AddIngredient_Implementation

        alt HasAuthority - Server
            Wrapper->>Wrapper: if not HasAuthority return
            Wrapper->>Wrapper: OverlappingActors.AddUnique

            alt 기존 재료 타입 존재
                Wrapper->>Wrapper: Stack.Quantity++
            else 새로운 재료 타입
                Wrapper->>Wrapper: OnAreaIngredients.Add
            end

            Wrapper->>Network: OnAreaIngredients 자동 복제
            Network-->>Wrapper: OnRep_AddIng All Clients
        else Client
            Note over Wrapper: RPC queued
        end
    end
```

### 2. 재료 제거 플로우 (내 구현: RemoveIngredient)

```mermaid
sequenceDiagram
    participant External as 외부 시스템(Player팀)
    participant Ingredient as AIngredientBase(내 구현)
    participant Wrapper as AWrappingPaper(내 구현)
    participant Network as Replication

    Note over External: 플레이어가 재료를 다시 집음
    External->>Ingredient: OnGrabbed 호출
    Note over Ingredient,Wrapper: 재료가 WrappingPaper에서 벗어남

    rect rgb(200, 255, 200)
        Note over Wrapper: 내가 구현한 RemoveIngredient 로직
        Ingredient->>Wrapper: OnComponentEndOverlap
        Wrapper->>Wrapper: RemoveIngredient_Implementation

        alt HasAuthority - Server
            Wrapper->>Wrapper: if not HasAuthority return
            Wrapper->>Wrapper: OverlappingActors.Remove

            loop OnAreaIngredients 순회
                Wrapper->>Wrapper: IngredientId 일치 검색
                alt 발견
                    Wrapper->>Wrapper: Stack.Quantity--
                    alt Quantity is 0
                        Wrapper->>Wrapper: RemoveAt
                    end
                end
            end

            Wrapper->>Network: OnAreaIngredients 자동 복제
            Network-->>Wrapper: OnRep_AddIng All Clients
        else Client
            Note over Wrapper: RPC queued
        end
    end
```

### 3. 햄버거 포장 플로우 (내 구현: TryWrap, CompleteWrapping, FindMatchingRecipe)

```mermaid
sequenceDiagram
    participant External as 외부 시스템(Player팀)
    participant Wrapper as AWrappingPaper(내 구현)
    participant DataTable as BurgerDataTable(내가 정의)
    participant Burger as AHamburger(내 구현)
    participant Network as Replication

    Note over External: InteractComponent가 WrappingPaper 감지
    External->>Wrapper: TryWrap Server RPC

    rect rgb(200, 255, 200)
        Note over Wrapper: 내가 구현한 TryWrap 로직
        alt HasAuthority - Server
            Wrapper->>Wrapper: if not HasAuthority return

            Note over Wrapper: 1. 유효성 검사
            Wrapper->>Wrapper: HasBreadPair
            Note over Wrapper: TopBread and BottomBread 체크
            Wrapper-->>Wrapper: return bool

            Wrapper->>Wrapper: HasExtraIngredient
            Note over Wrapper: 빵 외 재료 존재 체크
            Wrapper-->>Wrapper: return bool

            alt Valid Ingredients
                Note over Wrapper: 2. 레시피 매칭 시작
                Wrapper->>Wrapper: CompleteWrapping
                Wrapper->>Wrapper: FindMatchingRecipe

                Wrapper->>DataTable: GetAllRows
                DataTable-->>Wrapper: TArray FBurgerRecipe

                Note over Wrapper: 3. TMap 변환 및 비교
                Wrapper->>Wrapper: WrapMap MakeMapFromArray

                loop Each Recipe in DataTable
                    Wrapper->>Wrapper: RecipeMap MakeMapFromArray
                    Wrapper->>Wrapper: Compare Num

                    loop 각 재료
                        Wrapper->>Wrapper: WrapQty Find Key
                        Wrapper->>Wrapper: Compare Qty
                    end

                    alt 모든 재료 일치
                        Wrapper-->>Wrapper: return BurgerName
                    end
                end

                alt No Match
                    Wrapper-->>Wrapper: return WrongBurger
                end

                Note over Wrapper: 4. 햄버거 스폰 및 정리
                Wrapper->>Burger: SpawnActor
                Wrapper->>Burger: SetName
                Note over Burger: SetName HasAuthority 체크
                Burger->>Network: BurgerName 자동 복제

                Wrapper->>Wrapper: DestroyIngredients
                loop Each OverlappingActor
                    Wrapper->>Wrapper: Actor Destroy
                end
                Wrapper->>Wrapper: OnAreaIngredients Empty
                Wrapper->>Wrapper: Destroy self

                Network-->>Burger: 모든 클라이언트에 복제
            else Invalid Ingredients
                Note over Wrapper: 포장 실패 PRINTLOG
            end
        else Client
            Note over Wrapper: RPC queued
        end
    end
```

---

## 상태 다이어그램 - WrappingPaper 생명주기

```mermaid
stateDiagram-v2
    [*] --> Empty: BeginPlay()

    Empty --> CollectingIngredients: 첫 재료 오버랩
    CollectingIngredients --> CollectingIngredients: AddIngredient()
    CollectingIngredients --> Empty: 모든 재료 제거
    CollectingIngredients --> ValidatingRecipe: TryWrap() 호출

    ValidatingRecipe --> CheckingBread: HasBreadPair()
    CheckingBread --> CheckingExtra: Top & Bottom 존재
    CheckingBread --> CollectingIngredients: 빵 부족

    CheckingExtra --> MatchingRecipe: 추가 재료 존재
    CheckingExtra --> CollectingIngredients: 빵만 존재

    MatchingRecipe --> SpawningBurger: 레시피 매칭 성공
    MatchingRecipe --> SpawningWrongBurger: 레시피 불일치

    SpawningBurger --> DestroyingActors: SpawnActor(Burger)
    SpawningWrongBurger --> DestroyingActors: SpawnActor(WrongBurger)

    DestroyingActors --> CleaningUp: DestroyIngredients()
    CleaningUp --> [*]: Destroy(self)

    note right of ValidatingRecipe
        Server Authority만
        실행 가능
    end note

    note right of SpawningBurger
        BurgerName이
        네트워크 복제됨
    end note
```

---

## 데이터 플로우 다이어그램

```mermaid
flowchart TD
    Start([게임 시작]) --> SpawnWrapper[WrappingPaper 스폰]
    SpawnWrapper --> WaitOverlap{재료 오버랩 대기}

    WaitOverlap -->|재료를 놓음| OverlapEvent[OnComponentBeginOverlap]
    OverlapEvent --> ServerCheck1{HasAuthority?}
    ServerCheck1 -->|No Client| SendRPC1[AddIngredient RPC to Server]
    SendRPC1 --> ServerCheck1
    ServerCheck1 -->|Yes Server| AddToArray[OnAreaIngredients에 추가]

    AddToArray --> Replicate1[OnAreaIngredients 복제]
    Replicate1 --> OnRep[OnRep_AddIng All Clients]
    OnRep --> WaitOverlap

    WaitOverlap -->|재료를 집음| EndOverlap[OnComponentEndOverlap]
    EndOverlap --> ServerCheck2{HasAuthority?}
    ServerCheck2 -->|No Client| SendRPC2[RemoveIngredient RPC to Server]
    SendRPC2 --> ServerCheck2
    ServerCheck2 -->|Yes Server| RemoveFromArray[OnAreaIngredients에서 제거]
    RemoveFromArray --> Replicate2[OnAreaIngredients 복제]
    Replicate2 --> OnRep2[OnRep_AddIng All Clients]
    OnRep2 --> WaitOverlap

    WaitOverlap -->|포장 시도| TryWrapCall[TryWrap 호출]
    TryWrapCall --> ServerCheck3{HasAuthority?}
    ServerCheck3 -->|No Client| SendRPC3[TryWrap RPC to Server]
    SendRPC3 --> ServerCheck3
    ServerCheck3 -->|Yes Server| ValidateBread{HasBreadPair?}

    ValidateBread -->|No| WaitOverlap
    ValidateBread -->|Yes| ValidateExtra{HasExtraIngredient?}
    ValidateExtra -->|No| WaitOverlap
    ValidateExtra -->|Yes| CompleteWrap[CompleteWrapping]

    CompleteWrap --> LoadDataTable[(BurgerDataTable 로드)]
    LoadDataTable --> LoopRecipes{각 레시피 순회}

    LoopRecipes --> ConvertToMap[레시피를 TMap으로 변환]
    ConvertToMap --> CompareIngredients{재료 일치?}
    CompareIngredients -->|No| LoopRecipes
    CompareIngredients -->|Yes| FoundRecipe[BurgerName 반환]

    LoopRecipes -->|모두 불일치| WrongRecipe[WrongBurger 반환]

    FoundRecipe --> SpawnBurger[AHamburger 스폰]
    WrongRecipe --> SpawnBurger

    SpawnBurger --> SetBurgerName[Burger.SetName]
    SetBurgerName --> ServerSetName[ServerSetName RPC]
    ServerSetName --> ReplicateName[BurgerName 복제]

    ReplicateName --> DestroyIngs[DestroyIngredients]
    DestroyIngs --> LoopDestroy{각 재료 파괴}
    LoopDestroy --> DestroyActor[Actor.Destroy]
    DestroyActor --> LoopDestroy
    LoopDestroy -->|모두 파괴| DestroySelf[WrappingPaper.Destroy]
    DestroySelf --> End([프로세스 종료])

    style ServerCheck1 fill:#ff9999
    style ServerCheck2 fill:#ff9999
    style ServerCheck3 fill:#ff9999
    style Replicate1 fill:#99ccff
    style Replicate2 fill:#99ccff
    style ReplicateName fill:#99ccff
    style LoadDataTable fill:#99ff99
```

---

## 네트워크 아키텍처 (내 구현)

### Authority 및 RPC 패턴

**내가 구현한 네트워크 패턴**: Server Authority + Replication

```mermaid
graph TB
    subgraph Server["Listen Server - 내가 구현"]
        ServerWrapper[AWrappingPaper Server]
        ServerAuth{HasAuthority Check}
        ServerLogic[게임 로직 실행]
        ServerReplicate[OnAreaIngredients 복제]
    end

    subgraph Client1["Client 1"]
        Client1Wrapper[AWrappingPaper Client]
        Client1RPC[Server RPC 호출]
        Client1OnRep[OnRep_AddIng]
    end

    subgraph Client2["Client 2"]
        Client2Wrapper[AWrappingPaper Client]
        Client2RPC[Server RPC 호출]
        Client2OnRep[OnRep_AddIng]
    end

    Client1RPC -->|AddIngredient| ServerWrapper
    Client2RPC -->|RemoveIngredient| ServerWrapper

    ServerWrapper --> ServerAuth
    ServerAuth --> ServerLogic
    ServerLogic --> ServerReplicate

    ServerReplicate -->|Replicate| Client1Wrapper
    ServerReplicate -->|Replicate| Client2Wrapper

    Client1Wrapper --> Client1OnRep
    Client2Wrapper --> Client2OnRep

    style ServerAuth fill:#ffcccc
    style ServerLogic fill:#ccffcc
    style ServerReplicate fill:#ccccff
```

### 레플리케이션 설정 (내 구현)

**AWrappingPaper (본인 구현)**:

| 변수/함수 | 타입 | 방향 | 설명 (내 구현 내용) |
|----------|------|------|------|
| `OnAreaIngredients` | Replicated (OnRep) | Server → Clients | 재료 목록 자동 동기화 |
| `AddIngredient()` | Server RPC | Overlap Event → Server | 재료 추가 (UE5 Overlap 이벤트 자동 호출) |
| `RemoveIngredient()` | Server RPC | Overlap Event → Server | 재료 제거 (UE5 Overlap 이벤트 자동 호출) |
| `TryWrap()` | Server RPC | External → Server | 포장 시도 (InteractComponent에서 호출) |
| `bReplicates` | Property | - | 생성자에서 `true` 설정 |

**AIngredientBase (본인 구현)**:

| 변수/함수 | 타입 | 방향 | 설명 (내 구현 내용) |
|----------|------|------|------|
| `IngType` | Replicated | Server → Clients | 재료 타입 (EIngredient) 자동 동기화 |
| `OnGrabbed()` | 인터페이스 구현 | - | IGrabableProps 인터페이스 구현 (Player팀 요구사항) |
| `OnPut()` | 인터페이스 구현 | - | IGrabableProps 인터페이스 구현 |

**AHamburger (본인 구현)**:

| 변수/함수 | 타입 | 방향 | 설명 (내 구현 내용) |
|----------|------|------|------|
| `BurgerName` | Replicated | Server → Clients | 햄버거 이름 (레시피 매칭 결과) 동기화 |
| `ServerSetName()` | Server RPC | - | SetName()에서 Client일 때 자동 호출 |
| `SetName()` | Public Method | - | Authority 체크 후 분기 처리 |

---

## 주요 알고리즘 (내가 구현)

### 1. 레시피 매칭 알고리즘 (FindMatchingRecipe) - 핵심 로직

```cpp
// 의사 코드
function FindMatchingRecipe(DataTable, WrapperIngredients):
    WrapperMap = ConvertToMap(WrapperIngredients)  // O(n)

    AllRecipes = DataTable.GetAllRows()  // O(r), r = 레시피 수

    for each Recipe in AllRecipes:  // O(r)
        RecipeMap = ConvertToMap(Recipe.Ingredients)  // O(m), m = 재료 수

        if RecipeMap.Count != WrapperMap.Count:
            continue

        isMatched = true
        for each (Key, Value) in RecipeMap:  // O(m)
            if WrapperMap[Key] != Value:
                isMatched = false
                break

        if isMatched:
            return Recipe.BurgerName

    return WrongBurger

// 시간 복잡도: O(r * m)
// 공간 복잡도: O(m) - TMap 저장
```

### 2. 재료 수량 관리 (AddIngredient) - 내가 구현한 중복 체크 로직

```cpp
// WrappingPaper.cpp:57-107 실제 구현
void AWrappingPaper::AddIngredient_Implementation(...)
{
    if (!HasAuthority()) return;  // 서버만 실행

    OverlappingActors.AddUnique(OtherActor);  // WeakPtr 저장

    AIngredientBase* OtherIngredient = Cast<AIngredientBase>(OtherActor);

    if (OtherIngredient != nullptr)
    {
        const EIngredient IngId = OtherIngredient->GetIngType();

        // 핵심 로직: 기존 재료 타입 중복 체크
        for (FIngredientStack& tmp : OnAreaIngredients)
        {
            if (tmp.IngredientId == IngId)
            {
                tmp.Quantity++;  // 수량만 증가
                return;  // Early return으로 성능 최적화
            }
        }

        // 새로운 재료 타입 추가
        FIngredientStack Prop;
        Prop.IngredientId = IngId;
        Prop.Quantity = 1;
        OnAreaIngredients.Add(Prop);
    }
    // OnAreaIngredients는 Replicated이므로 자동 동기화
}

// 시간 복잡도: O(n), n = 재료 종류 수 (평균 3~7개, 최대 16)
// 공간 복잡도: O(n) - OnAreaIngredients 배열
```

---

## 의존성 그래프 (내 구현 범위)

```mermaid
graph TB
    subgraph MySystem["내가 구현한 시스템"]
        WrappingPaper[AWrappingPaper]
        Hamburger[AHamburger]
        Ingredient[AIngredientBase]
        BurgerData[BurgerData.h]
        DataTable[(BurgerDataTable)]
    end

    subgraph External["외부 시스템 - Player팀"]
        Grabable[IGrabableProps]
        Interact[UInteractComponent]
    end

    subgraph Engine["UE5 엔진"]
        Network[Replication]
        UE5Collision[Collision]
    end

    WrappingPaper -->|spawns| Hamburger
    WrappingPaper -->|detects| Ingredient
    WrappingPaper -->|reads| DataTable
    WrappingPaper -->|includes| BurgerData
    Hamburger -->|includes| BurgerData
    Ingredient -->|includes| BurgerData

    Hamburger -.->|implements| Grabable
    Ingredient -.->|implements| Grabable

    WrappingPaper -->|uses| Network
    Hamburger -->|uses| Network
    Ingredient -->|uses| Network

    WrappingPaper -->|uses| UE5Collision

    Interact -.->|calls| WrappingPaper
    Interact -.->|calls| Ingredient

    style WrappingPaper fill:#ccffcc,stroke:#00aa00,stroke-width:3px
    style Hamburger fill:#ccffcc,stroke:#00aa00
    style Ingredient fill:#ccffcc,stroke:#00aa00
    style BurgerData fill:#ccffcc,stroke:#00aa00
    style DataTable fill:#99ff99,stroke:#00aa00
    style Grabable fill:#dddddd
    style Interact fill:#dddddd
    style Network fill:#ccccff
    style UE5Collision fill:#ccccff
```

---

## 핵심 설계 결정 사항 (내가 내린 결정)

### 1. **서버 Authority 패턴 선택**
- **내가 내린 결정**: 모든 게임 로직을 서버에서만 실행
- **이유**:
  - 멀티플레이 환경에서 클라이언트 간 재료 수량 동기화 보장
  - 레시피 매칭 로직이 서버에서만 실행되어 일관성 유지
  - 클라이언트는 OnRep 콜백으로 UI만 업데이트
- **구현 방법**: 모든 핵심 함수에 `if (!HasAuthority()) return;` 추가
  - `AddIngredient_Implementation()` (WrappingPaper.cpp:61)
  - `RemoveIngredient_Implementation()` (WrappingPaper.cpp:113)
  - `TryWrap_Implementation()` (WrappingPaper.cpp:176)
  - `CompleteWrapping()` (WrappingPaper.cpp:304)

### 2. **Server RPC만 사용 (NetMulticast 미사용)**
- **내가 내린 결정**: WrappingPaper는 Server RPC만 사용
- **이유**:
  - 상태 변경(OnAreaIngredients)만 필요, 시각 효과 없음
  - Replicated 변수가 자동으로 클라이언트 업데이트
  - NetMulticast는 불필요 → 네트워크 트래픽 감소
- **비교**: InteractComponent(Player팀)는 MulticastRPC 사용
  - 이유: 즉각적인 집기/놓기 애니메이션 필요
  - 내 시스템은 "상태 관리"만 담당하므로 다른 패턴 선택

### 3. **TArray vs TMap for Ingredients (내가 선택한 자료구조)**
- **내가 내린 결정**: 내부 저장은 `TArray<FIngredientStack>`, 레시피 비교 시에만 TMap 변환
- **이유**:
  - **Replication 호환성**: UE5는 TMap 복제가 복잡함, TArray는 간단
  - **재료 수 적음**: 평균 3~7개, 최대 16개 → 순회 비용 무시 가능
  - **메모리 효율**: TMap의 해시 테이블 오버헤드 불필요
- **구현 코드** (WrappingPaper.cpp:209-220):
  ```cpp
  TMap<EIngredient, int32> AWrappingPaper::MakeMapFromArray(const TArray<FIngredientStack>& InArray)
  {
      TMap<EIngredient, int32> Result;
      for (const FIngredientStack& it : InArray)
      {
          Result.FindOrAdd(it.IngredientId) += it.Quantity;
      }
      return Result;
  }
  ```
- **트레이드오프**: 레시피 매칭 시 O(n) 변환 비용 < Replication 단순성

### 4. **WeakObjectPtr 사용 (안전한 메모리 관리)**
- **내가 내린 결정**: `TArray<TWeakObjectPtr<AActor>> OverlappingActors`
- **이유**:
  - **외부 파괴 대응**: Player가 재료를 집었다가 다른 곳에서 파괴 가능
  - **댕글링 포인터 방지**: 파괴된 재료 접근 시 크래시 방지
  - **안전한 접근**: `Ptr.Get()` 체크로 유효성 확인
- **구현 코드** (WrappingPaper.cpp:333-341):
  ```cpp
  for (const TWeakObjectPtr<AActor>& Ptr : ActorsToDestroy)
  {
      if (AActor* Actor = Ptr.Get())  // 유효성 검사
      {
          Actor->Destroy();
      }
  }
  ```

### 5. **자체 파괴 패턴 (Self-Destroy) - 일회용 설계**
- **내가 내린 결정**: 포장 완료 후 WrappingPaper 자신을 파괴
- **이유**:
  - **게임 디자인**: 포장지는 일회용 소모품 (현실 반영)
  - **메모리 관리**: 사용 후 즉시 해제로 누수 방지
  - **재사용 불가**: 초기화보다 재생성이 더 안전
- **구현** (WrappingPaper.cpp:341): `Destroy()`
- **주의사항**: Destroy() 호출 후 어떤 멤버에도 접근 금지

---

## 성능 고려사항

### 1. **틱 함수 최적화**
```cpp
void AWrappingPaper::Tick(float DeltaTime)
{
    // 디버그 로그만 출력 (게임 로직 없음)
    if (bShowLog)  // 조건부 실행
    {
        PrintLog();  // DrawDebugString (에디터 전용)
    }
}
```
- **개선 가능**: `bShowLog == false` 시 Tick 비활성화 고려

### 2. **레플리케이션 대역폭**
- **현재**: `OnAreaIngredients` 전체 배열 복제
- **문제**: 재료 추가/제거마다 전체 배열 전송
- **개선안**: Custom NetSerialize 구현 (Delta Compression)

### 3. **DataTable 조회**
```cpp
// 매번 GetAllRows 호출 (O(r))
DT->GetAllRows<FBurgerRecipe>(TEXT("FBurgerRecipe"), AllRows);
```
- **문제**: 포장 시도마다 전체 테이블 로드
- **개선안**: BeginPlay에서 캐싱 (TMap<재료조합, 버거이름>)

---

## 확장 가능성

### 1. **추가 기능 제안**
- [ ] 포장 애니메이션 (Timeline 사용)
- [ ] 포장 실패 시 피드백 (사운드/이펙트)
- [ ] 부분 포장 (절반 완료 상태 저장)
- [ ] 포장지 재사용 (실패 시 파괴 안 함)

### 2. **새로운 레시피 추가**
```cpp
// DataTable에 행만 추가하면 됨 (코드 수정 불필요)
// 1. Content/Data/BurgerRecipes.uasset 열기
// 2. Add Row
// 3. BurgerName, Ingredients 설정
```

### 3. **특수 재료 지원**
- 현재: 모든 재료가 EIngredient enum
- 확장: `ASpecialIngredient` 서브클래스 (시간제한, 부패 등)
- 필요 변경: `AddIngredient`에서 특수 로직 처리

---

## 알려진 이슈 및 TODO

### 현재 상태
✅ 멀티플레이 네트워크 동기화 완료 (Commit 88662b1)
✅ Server RPC 패턴 구현
✅ Authority 체크 추가
✅ OnAreaIngredients 레플리케이션

### TODO 항목
```cpp
// WrappingPaper.h:17
// TODO(human): Listen 서버 멀티플레이 구현
// → 상태: 완료됨 (최신 커밋에서 구현)

// WrappingPaper.cpp:13-14
// TODO(human): 이 액터가 네트워크를 통해 복제되도록 설정
// → 상태: 완료됨 (bReplicates = true)

// WrappingPaper.cpp:59, 111, 174, 302
// TODO(human): 서버에서만 실행되도록 보장
// → 상태: 완료됨 (HasAuthority 체크 추가)
```

### 잠재적 버그
1. **레이스 컨디션**: 여러 클라이언트가 동시에 TryWrap 호출
   - **현상**: 햄버거 중복 생성 가능성
   - **해결**: Server에서 상태 체크로 방지됨
   - **주의**: WrappingPaper가 이미 Destroy 중일 때 TryWrap 호출 가능성

2. **네트워크 지연**: 재료 추가 후 즉시 포장 시도
   - **현상**: 클라이언트에서 OnAreaIngredients 미동기화
   - **해결**: Server RPC 사용으로 서버만 판단

3. **재료 파괴 타이밍**: DestroyIngredients 중 오버랩 이벤트
   - **현상**: 이미 파괴된 액터에 접근
   - **해결**: WeakObjectPtr + 유효성 검사

4. **IngContainer 예외 처리**:
   - **발견**: InteractComponent.cpp:53-57에서 IngContainer 특수 처리
   - **동작**: Container에서 Ingredient를 가져와 직접 스폰
   - **주의**: 이 경로는 문서의 주 흐름과 다름 (별도 시스템)

---

## 참고 자료

### 관련 파일
- `Source/MHGA/Public/WrappingPaper.h`
- `Source/MHGA/Private/WrappingPaper.cpp`
- `Source/MHGA/Public/Hamburger.h`
- `Source/MHGA/Public/BurgerData.h`
- `Source/MHGA/Public/Ingredient/IngredientBase.h`
- `Source/MHGA/Public/Player/InteractComponent.h`

### 관련 시스템
- [InteractComponent 문서](링크 필요)
- [Hamburger 시스템](링크 필요)
- [Ingredient 시스템](링크 필요)
- [네트워크 아키텍처](../CLAUDE.md#멀티플레이-네트워크-아키텍처)

### Unreal Engine 참고
- [Network Replication](https://docs.unrealengine.com/5.6/en-US/networking-overview-for-unreal-engine/)
- [Server RPC](https://docs.unrealengine.com/5.6/en-US/rpcs-in-unreal-engine/)
- [DataTable](https://docs.unrealengine.com/5.6/en-US/data-driven-gameplay-elements-in-unreal-engine/)

---

**문서 버전**: 1.0
**최종 업데이트**: 2025-10-24
**작성자**: MHGA Development Team
**검토 상태**: Initial Draft
