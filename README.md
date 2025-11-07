<div align="center">

# MHGA

</div>

## 📑 목차

- [프로젝트 개요](#-프로젝트-개요)
- [핵심 시스템 구조](#-핵심-시스템-구조)
  - [네트워크 아키텍처](#1-네트워크-아키텍처)
  - [AI 손님 시스템](#2-ai-손님-시스템)
  - [조리 시스템](#3-조리-시스템)
  - [포장 및 레시피 검증](#4-포장-및-레시피-검증-시스템)
  - [POS 시스템](#5-pospoint-of-sale-시스템)
  - [상호작용 시스템](#6-상호작용-시스템)
  - [캐릭터 및 입력](#7-캐릭터-및-입력-시스템)
- [포트폴리오 핵심 항목](#-포트폴리오-핵심-항목)
- [기술적 하이라이트](#-기술적-하이라이트)
- [게임 플로우](#-게임-플로우-및-시나리오)
- [기술 스택](#-사용-기술-스택)
- [성능 최적화](#-성능-최적화)
- [학습 포인트](#-학습-포인트-및-도전-과제)
- [향후 개선](#-향후-개선-방향)
- [코드 구조](#-코드-구조)
- [포트폴리오 강조 포인트](#-포트폴리오-강조-포인트)

---

## 📋 프로젝트 개요

**MHGA**는 실제 햄버거 가게의 아르바이트 환경을 시뮬레이션한 협동 게임입니다. 플레이어는 손님 주문 접수부터 재료 조리, 햄버거 제작, 음식 전달까지 전체 프로세스를 경험하며, 동료들과 협력하여 몰려드는 손님들을 효율적으로 응대해야 합니다.

### 개발 환경
- **엔진**: Unreal Engine 5.6
- **개발 언어**: C++ / Blueprint
- **네트워크**: Listen Server 기반 멀티플레이
- **개발 인원**: 3명 (김홍범, 황규환, 허지웅)

### 프로젝트 컨셉 (README 기반)
- **햄버거 아르바이트 시뮬레이션**
- 햄버거 가게의 아르바이트를 계산부터 음식 조리까지 경험
- 동료들과 협심하여 손님들이 몰려드는 햄버거 가게 경영 모의실험
- 실제 근무 환경과 비슷한 환경 구성
- 레시피는 벽에 붙어있거나 암기해야 함

### 주요 특징
- ✅ **리슨 서버 기반 멀티플레이**: 협동 플레이 지원
- ✅ **AI 손님 시스템**: FSM 기반의 지능형 손님 행동 패턴
- ✅ **실제 레시피 기반 조리 시스템**: 데이터 테이블을 활용한 정확한 메뉴 구성
- ✅ **물리 기반 상호작용**: 재료 집기, 조리, 포장 등 모든 작업이 물리 시뮬레이션 기반
- ✅ **3D UI 시스템**: Widget Interaction을 활용한 POS 시스템
- ✅ **동적 손님 관리**: 최대 손님 수 제한 및 대기열 시스템

---

## 🎮 핵심 시스템 구조

### 1. 네트워크 아키텍처

**리슨 서버 구조를 통한 효율적인 멀티플레이 구현**

#### 네트워크 구조 다이어그램

```mermaid
graph TB
    subgraph "Listen Server"
        H["🎮 Host Player<br/>(Server Authority)"]
        GM["⚙️ GameMode<br/>(Logic)"]
        GS["📊 GameState<br/>(Replicate)"]
    end

    subgraph "Network"
        RPC["🔄 RPC 호출"]
        REP["📡 Replication"]
    end

    subgraph "Client 1"
        C1["👤 Player 1<br/>(Local Control)"]
        UIC1["🎨 UI/Cache"]
    end

    subgraph "Client 2"
        C2["👤 Player 2<br/>(Local Control)"]
        UIC2["🎨 UI/Cache"]
    end

    H -->|Authority| GM
    GM -->|Updates| GS
    GS -->|REP| REP
    C1 -->|RPC| RPC
    C2 -->|RPC| RPC
    RPC -->|ServerRPC| GM
    GM -->|MulticastRPC| RPC
    REP -->|Sync| UIC1
    REP -->|Sync| UIC2

    style H fill:#FFB6C6
    style GM fill:#87CEEB
    style GS fill:#98FB98
    style C1 fill:#FFE4B5
    style C2 fill:#FFE4B5
```

#### 주요 구현 사항
- **Replication 시스템**: Actor 및 Component 레벨에서의 데이터 동기화
- **RPC(Remote Procedure Call)**: Server/Client/Multicast RPC를 활용한 상태 전파
- **Authority 기반 로직 분리**: 서버에서만 실행되어야 하는 게임 로직 보호

```cpp
// GameState에서 Counter POS 복제
void AMHGAGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMHGAGameState, Counter);
}
```

#### 네트워크 동기화 전략
- **재료 및 Props**: `bReplicates = true` + `SetReplicateMovement(true)`로 물리 기반 동작 동기화
  ```cpp
  // IngredientBase 생성자
  AIngredientBase::AIngredientBase()
  {
      bReplicates = true;
      AActor::SetReplicateMovement(true);
  }
  
  // BeginPlay에서 클라이언트 물리 비활성화
  void AIngredientBase::BeginPlay()
  {
      if (HasAuthority() == false)
      {
          Mesh->SetSimulatePhysics(false);
          Mesh->SetEnableGravity(false);
      }
  }
  ```
- **UI 상호작용**: PlayerController를 통한 Server RPC → Counter Actor의 Multicast RPC 체인
- **손님 AI**: 서버에서만 FSM 로직 실행, 결과만 복제

---

### 2. AI 손님 시스템

**유한 상태 머신(FSM) 기반의 손님 행동 AI**

#### 손님 종류 및 성격 시스템

**손님 성격 (ECustomerPersonality)**
```cpp
UENUM(BlueprintType)
enum class ECustomerPersonality : uint8
{
    Standard    UMETA(DisplayName = "표준"),
    Polite      UMETA(DisplayName = "공손함"),
    Rude        UMETA(DisplayName = "무례함"),
    Impatient   UMETA(DisplayName = "급함"),
    Special_VIP UMETA(DisplayName = "매그너스")  // 특별 손님
};
```

**특별 손님 시스템**
- **생성 확률**: `SpecialCustomerChance = 0.1f` (10%)
- **특별 주문**: `EBurgerMenu::WrongBurger` (항상 잘못된 버거 주문)
- **전용 메쉬 및 애니메이션**: `specialVisual` 사용
- **메쉬 인덱스**: -1 (일반 손님과 구분)

```cpp
// CustomerFSM::BeginPlay에서 손님 타입 결정
if (FMath::FRand() < SpecialCustomerChance)
{
    // 특별 손님
    personality = ECustomerPersonality::Special_VIP;
    SelectedMeshIndex = -1;  // 특별 손님 메쉬
}
else
{
    // 일반 손님
    int32 NumRegularPersonalities = static_cast<int32>(ECustomerPersonality::Special_VIP);
    personality = static_cast<ECustomerPersonality>(FMath::RandRange(0, NumRegularPersonalities - 1));
    SelectedMeshIndex = FMath::RandRange(0, me->regularVisuals.Num() - 1);
}
```

#### 손님 상태 전환 다이어그램

```mermaid
graph TD
    A["🚶 입장<br/>(Spawn)"] --> B{"대기열<br/>자리?"}
    B -->|Yes| C["➡️ 대기열 이동<br/>(GoingToLine)"]
    B -->|No| D["🔄 배회<br/>(Wandering)"]
    C --> E["⏳ 줄 서서 대기<br/>(WaitingInLine)"]
    D --> E
    E --> F["🗣️ 주문 중<br/>(Ordering)"]
    F --> G["⏳ 음식 대기<br/>(WaitingForFood)"]
    G --> H{"배회<br/>필요?"}
    H -->|Yes| D
    H -->|No| I["➡️ 픽업존 이동<br/>(GoingToPickup)"]
    D --> I
    I --> J["⏳ 픽업 대기<br/>(WaitingForPickup)"]
    J --> K["✓ 음식 검증<br/>& 수령"]
    K --> L["🚪 퇴장<br/>(Exit)"]

    style A fill:#FFE4B5
    style F fill:#87CEEB
    style K fill:#98FB98
    style L fill:#DDA0DD
```

#### FSM 구조 (CustomerFSM)
총 9가지 상태를 가진 유한 상태 머신으로 손님의 행동을 제어합니다.

```cpp
enum class EAIState : uint8
{
    None,              // 초기 상태
    GoingToLine,       // 줄 서러 이동 중
    WaitingInLine,     // 줄 서서 대기 중
    Wandering,         // 배회 중
    Ordering,          // 주문 중
    WaitingForFood,    // 음식 대기 중
    GoingToPickup,     // 픽업존 이동 중
    WaitingForPickup,  // 픽업존 대기 중
    Exit               // 퇴장
};
```

#### 주요 기능

**1. 대기열 관리 시스템**
- 빈 자리가 있으면 즉시 줄서기, 없으면 배회하며 대기
- 손님이 주문 완료 시 자동으로 대기열 앞당김
- 대기 중인 손님을 순차적으로 호출

```cpp
void ACustomerManager::UpdateWaitingPosition()
{
    for (int32 i = 1; i < waitingCustomers.Num(); ++i)
    {
        if (waitingCustomers[i] != nullptr && waitingCustomers[i - 1] == nullptr)
        {
            // 앞자리로 이동 명령
            ACustomerAI* customerToMove = waitingCustomers[i];
            customerToMove->fsm->MoveToTarget(waitingPoints[i-1]);
            waitingCustomers[i-1] = waitingCustomers[i];
            waitingCustomers[i] = nullptr;
        }
    }
}
```

**2. 배회 시스템**
- NavigationSystem을 활용한 NavMesh 기반 랜덤 이동
- 3~5초 간격으로 새로운 목적지 설정
- 타이머를 활용한 비동기 처리

**3. 주문 시스템**
- Enum 기반 메뉴 선택 (BigMac, BTD, QPC, Shanghai, Shrimp)
- UMG 위젯을 통한 말풍선 UI 표시
- 주문 완료 시 자동으로 음식 대기 상태로 전환

**4. 음식 검증 시스템**
- 픽업존에서 음식 이름과 주문한 메뉴 비교
- 일치 시 만족 표시, 불일치 시 불만 표시
- 평점 시스템 연동 (ShowScoreFeedback 함수로 구현)

```cpp
void UCustomerFSM::CheckAndTakeFood()
{
    if (MyPickupZone->HasFood())
    {
        AHamburger* TakenHamburger = Cast<AHamburger>(MyPickupZone->TakeFood());
        FString OrderedMenuName = // Enum을 String으로 변환
        FString TakenBurgerName = TakenHamburger->GetBurgerName();

        if (OrderedMenuName == TakenBurgerName)
        {
            me->ShowScoreFeedback(EScoreChangeReason::CorrectFood);  // 만족
        }
        else
        {
            me->ShowScoreFeedback(EScoreChangeReason::WrongFood);  // 불만
        }
    }
}
```

**5. 손님 매니저 (CustomerManager)**
- 타이머 기반 자동 스폰 시스템
- 최대 인원 제한 관리 (MaxSpawnedCustomers)
- 픽업 대기열 관리 및 호출 시스템

---

### 3. 조리 시스템

**물리 기반 상호작용과 상태 머신을 결합한 조리 메커니즘**

#### 재료 시스템 (Ingredient System)

**재료 클래스 계층 구조**
```
IngredientBase (기본 재료 클래스, IngType = None)
├── BottomBread (IngType = BottomBread)
├── MiddleBread (IngType 미설정)
├── TopBread (IngType = TopBread)
├── Patty (IngType = Patty, 조리 상태 포함)
├── Cheese (IngType = Cheese)
├── Lettuce (IngType 미설정)
├── Tomato (IngType 미설정)
├── Onion (IngType = Onion)
├── Pickle (IngType = Pickle)
└── SauceBottle (IngType = None, 발사 기능)
```

**주요 구현 재료**
- `BottomBread`, `TopBread`: 햄버거 상하 빵
- `Patty`: 3단계 조리 상태 (Raw → Cooked → Overcooked)
- `Cheese`, `Onion`, `Pickle`: 토핑 재료
- `SauceBottle`: Line Trace 기반 소스 발사

**재료 Enum 정의**
```cpp
enum class EIngredient : uint8
{
    None, BottomBread, MiddleBread, TopBread,
    Patty, Lettuce, Tomato, Onion,
    Cheese, Pickle, Sauce, ShanghaiPortion, ShrimpPortion
};
```

#### 패티 조리 시스템

**Patty 클래스** 및 **CookingArea**를 통한 3단계 조리 상태 구현

```cpp
enum class EPattyCookState : uint8
{
    Raw,        // 날것
    Cooked,     // 적절히 구워짐
    Overcooked  // 타버림
};
```

**패티 조리의 특징**
- **앞뒷면 개별 관리**: 패티의 앞면과 뒷면을 각각 따로 요리
- **뒤집기 메커니즘**: Flip() 함수로 앞뒷면 상태를 전환
- **타이머 기반 조리**: cookTime(3초)과 overcookTime(5초) 기본값
- **상태별 텍스처 변경**: Raw → Cooked → Overcooked 상태에 따라 텍스처 동적 변경

**패티 조리 타임라인**

```mermaid
timeline
    title 패티 조리 과정 (한쪽 면 기준)
    section 상태 변화
        Raw (날것) : 시작 : 원본 텍스처
        0초 : 조리 영역 진입 : StartCook() 호출
        3초 : Cooked 상태 전환 : 색상 변경 (cookTime)
        8초 : Overcooked 상태 전환 : 검은색 텍스처 (overcookTime 추가)
    section 조리 상태
        현재면 조리 중 : 3~8초 : bCurrentSideCooked = true
        현재면 오버쿡 : 8초+ : bCurrentSideOverCooked = true
```

**패티 조리 상태 업데이트 로직**
```cpp
// Patty.h - 앞뒷면 상태 추적
UPROPERTY(ReplicatedUsing = OnRep_CookStateChanged)
bool bIsFrontCooked;      // 앞면 구워짐
UPROPERTY(ReplicatedUsing = OnRep_CookStateChanged)
bool bIsBackCooked;       // 뒷면 구워짐
UPROPERTY(ReplicatedUsing = OnRep_CookStateChanged)
bool bIsFrontOverCooked;  // 앞면 오버쿡
UPROPERTY(ReplicatedUsing = OnRep_CookStateChanged)
bool bIsBackOverCooked;   // 뒷면 오버쿡

// 종합 상태 계산
UPROPERTY(Replicated)
EPattyCookState CookState;  // Raw / Cooked / Overcooked

// CookingArea와의 상호작용
void ACookingArea::OnOverlapBegin(...)
{
    AIngredientBase* ingredient = Cast<AIngredientBase>(OtherActor);
    if (ingredient)
        ingredient->StartCook();  // 패티의 StartCook() 호출
}
```

#### 소스 발사 시스템

**SauceBottle 클래스**의 특수 기능
- Arrow Component로 발사 방향 시각화
- ShootSauce() 함수로 소스 인스턴스 스폰
- 실시간 물리 기반 소스 생성

```cpp
class ASauceBottle : public AIngredientBase
{
public:
    // 소스 발사 함수
    void ShootSauce();

    virtual void OnUse() override;  // 마우스 클릭 시 호출

protected:
    UPROPERTY(VisibleAnywhere)
    class UArrowComponent* Arrow;  // 발사 방향 표시

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class AIngredientBase> SauceClass;  // 스폰될 소스 클래스
};
```

---

### 4. 포장 및 레시피 검증 시스템

**WrappingPaper 클래스를 통한 햄버거 완성 및 검증**

#### 작동 원리

**1. 재료 추적**
- Box Collision을 통한 Overlap 이벤트 감지
- 재료 종류별 수량 카운트
- TArray<FIngredientStack> 구조로 관리

```cpp
struct FIngredientStack
{
    EIngredient IngredientId;
    int32 Quantity;
};
```

**2. 포장 조건 검증**
```cpp
bool AWrappingPaper::HasBreadPair() const
{
    // TopBread와 BottomBread가 각각 1개 이상 필요
    int32 Q_TBread = 0, Q_BBread = 0;
    for (const FIngredientStack& tmp : OnAreaIngredients)
    {
        if (tmp.IngredientId == EIngredient::TopBread) Q_TBread++;
        if (tmp.IngredientId == EIngredient::BottomBread) Q_BBread++;
    }
    return Q_TBread >= 1 && Q_BBread >= 1;
}

bool AWrappingPaper::HasExtraIngredient() const
{
    // 빵 외에 다른 재료가 1개 이상 필요
    int32 count = 0;
    for (const FIngredientStack& tmp : OnAreaIngredients)
    {
        if (!(tmp.IngredientId == EIngredient::BottomBread || 
              tmp.IngredientId == EIngredient::TopBread))
            count++;
    }
    return count > 0;
}
```

**3. 레시피 매칭 알고리즘**

DataTable을 활용한 정확한 레시피 비교

**레시피 매칭 프로세스**

```mermaid
graph TD
    A["📦 포장지에<br/>재료 배치 완료"] --> B["🔍 Wrapper<br/>재료 추출"]
    B --> C["📊 WrapMap<br/>TMap 생성<br/>Ingredient → Qty"]

    C --> D["📖 DataTable<br/>모든 레시피 로드"]
    D --> E["🔄 각 레시피<br/>반복 검증"]

    E --> F{"재료 수<br/>일치?"}
    F -->|No| E
    F -->|Yes| G{"모든 재료<br/>& 수량<br/>일치?"}

    G -->|No| E
    G -->|Yes| H["✅ 매칭 성공<br/>버거명 반환"]

    E --> I{"더 이상<br/>레시피?"}
    I -->|Yes| J["❌ 매칭 실패<br/>WrongBurger 반환"]

    A --> K["예시: 빅맥<br/>Bottom Bread x1<br/>Patty x2<br/>Cheese x2<br/>Lettuce x1<br/>Top Bread x1"]

    H --> L["🍔 햄버거 생성<br/>& 영수증 출력"]
    J --> M["📝 주문 기록<br/>잘못된 음식"]

    style A fill:#FFE4B5
    style B fill:#87CEEB
    style C fill:#90EE90
    style D fill:#87CEEB
    style E fill:#DDA0DD
    style F fill:#FFB6C1
    style G fill:#FFB6C1
    style H fill:#98FB98
    style I fill:#FFB6C1
    style J fill:#FFB6C1
    style K fill:#F0E68C
    style L fill:#90EE90
    style M fill:#FFB6C1
```

**레시피 매칭 코드**

```cpp
EBurgerMenu AWrappingPaper::FindMatchingRecipe(
    UDataTable* DT,
    const TArray<FIngredientStack>& WrapperIngr)
{
    TMap<EIngredient, int32> WrapMap = MakeMapFromArray(WrapperIngr);
    TArray<FBurgerRecipe*> AllRows;
    DT->GetAllRows<FBurgerRecipe>(TEXT("FBurgerRecipe"), AllRows);

    for (FBurgerRecipe* Row : AllRows)
    {
        TMap<EIngredient, int32> RecipeMap = MakeMapFromArray(Row->Ingredients);

        // 재료 수가 다르면 스킵
        if (RecipeMap.Num() != WrapMap.Num()) continue;

        // 모든 재료와 수량이 정확히 일치하는지 검증
        bool isMatched = true;
        for (const auto& Pair : RecipeMap)
        {
            const int32* WrapQty = WrapMap.Find(Pair.Key);
            if (WrapQty == nullptr || *WrapQty != Pair.Value)
            {
                isMatched = false;
                break;
            }
        }

        if (isMatched) return Row->BurgerName;
    }

    return EBurgerMenu::WrongBurger;
}
```

**4. 햄버거 생성 및 정리**
```cpp
void AWrappingPaper::CompleteWrapping()
{
    // 레시피 매칭
    EBurgerMenu CreatedBurgerName = FindMatchingRecipe(
        BurgerDataTable, 
        OnAreaIngredients
    );
    
    // 햄버거 스폰
    AHamburger* SpawnedBurger = GetWorld()->SpawnActor<AHamburger>(
        BurgerClass, 
        this->GetActorTransform()
    );
    SpawnedBurger->SetName(BurgerMenuName);
    
    // 재료 및 포장지 정리
    DestroyIngredients();
}
```

#### 지원 메뉴
```cpp
enum class EBurgerMenu : uint8
{
    None,
    WrongBurger,  // 잘못된 조합
    BigMac,       // 빅맥
    BTD,          // 베이컨 토마토 디럭스
    QPC,          // 쿼터 파운드 치즈
    Shanghai,     // 상하이 버거
    Shrimp        // 새우 버거
};
```

---

### 5. POS(Point of Sale) 시스템

**3D Widget Interaction을 활용한 실시간 주문 관리**

#### 시스템 구조

**CounterPOS (Actor)**
- Static Mesh로 된 카운터 모델
- Widget Component를 통한 3D UI 표시
- 네트워크 복제를 통한 모든 클라이언트 동기화

```cpp
ACounterPOS::ACounterPOS()
{
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
    
    bReplicates = true;
    bAlwaysRelevant = true;
    WidgetComponent->SetIsReplicated(true);
}
```

#### 주문 관리 구조

**FOrderArray 구조체**
```cpp
struct FOrderArray
{
    ACustomerAI* Customer;           // 주문한 손님
    TArray<EBurgerMenu> Menus;       // 주문 메뉴 리스트
};

// CounterPOS에서 관리
TMap<int32, FOrderArray> OrderMap;  // 주문번호 → 주문정보
```

#### UI 구성

**1. CounterUI (메인 UI)**
- **CustomerCanvas**: 주문 완료 목록 표시
  - UniformGridPanel로 5열 그리드 구성
  - 각 주문은 CustomerButtonUI로 표현
  
- **OrderCanvas**: 메뉴 선택 및 주문 접수
  - MenuGrid: 동적으로 생성되는 메뉴 버튼
  - SelectedListBox: 현재 선택된 메뉴 리스트
  - 주문/삭제 버튼

**2. MenuButtonUI (메뉴 버튼)**
```cpp
void UMenuButtonUI::Init(const EBurgerMenu InName, UCounterUI* InOwner)
{
    MenuName = InName;
    CounterUI = InOwner;
    // Enum을 DisplayName으로 변환하여 표시
    TEXT_MenuName->SetText(
        StaticEnum<EBurgerMenu>()->GetDisplayNameTextByValue(
            static_cast<int64>(MenuName)
        )
    );
}
```

**3. CustomerButtonUI (주문 번호 버튼)**
```cpp
class UCustomerButtonUI : public UUserWidget
{
    int32 Num;                      // 주문 번호
    TArray<EBurgerMenu> Menus;      // 주문 메뉴
    ACustomerAI* Customer;          // 손님 참조 (미사용)
    
    void Init(TArray<EBurgerMenu> InMenu, int32 InNum, UCounterUI* InOwner);
};
```
- 주문 번호 표시
- 클릭 시 해당 주문의 메뉴 리스트 표시
- 준비 완료 처리 기능

#### 네트워크 동기화 흐름

**주문 접수 및 완료 프로세스**

```mermaid
graph TD
    subgraph "Client Action"
        A["👆 플레이어가<br/>메뉴 버튼 클릭"]
    end

    subgraph "Network RPC Chain"
        B["📤 ServerRPC<br/>AddMenuToList<br/>Client → Server"]
        C["✓ 서버 검증<br/>& 처리"]
        D["📢 MulticastRPC<br/>AddMenuToList<br/>Server → All"]
    end

    subgraph "Client Update"
        E["🎨 Client 1<br/>UI 업데이트"]
        F["🎨 Client 2<br/>UI 업데이트"]
    end

    subgraph "Order Completion"
        G["📝 주문 저장<br/>OrderMap"]
        H["🧾 영수증 생성<br/>ReceiptActor Spawn"]
        I["🤖 AI 손님<br/>상태 전환"]
    end

    A --> B
    B --> C
    C --> D
    D --> E
    D --> F
    C --> G
    G --> H
    G --> I

    style A fill:#FFE4B5
    style B fill:#87CEEB
    style C fill:#90EE90
    style D fill:#87CEEB
    style E fill:#FFE4B5
    style F fill:#FFE4B5
    style G fill:#F0E68C
    style H fill:#F0E68C
    style I fill:#DDA0DD
```

**주문 완료 상세 프로세스**
```cpp
void UCounterUI::OrderMenuBtnRPC()
{
    // 주문 저장
    PosActor->OrderMap.FindOrAdd(PosActor->OrderNum) = 
        {PosActor->GetCustomer(), OrderList};
    
    // 주문 번호 버튼 생성
    UCustomerButtonUI* NewCustomerBtn = CreateWidget<UCustomerButtonUI>(...);
    NewCustomerBtn->Init(OrderList, PosActor->OrderNum, this);
    CustomerGrid->AddChildToUniformGrid(NewCustomerBtn, Row, Col);
    
    // 영수증 출력 (서버에서만)
    if (PosActor->HasAuthority())
    {
        AReceiptActor* NewReceipt = GetWorld()->SpawnActor<AReceiptActor>(...);
        NewReceipt->MulticastRPC_Init(OrderNum, MenuStrings);
    }
    
    // AI 손님 주문 완료 처리
    if (PosActor->GetCustomer() != nullptr)
        PosActor->GetCustomer()->fsm->FinishOrder();
}
```

#### 영수증 시스템

**ReceiptActor 클래스**
- Canvas Render Target을 활용한 동적 텍스트 렌더링
- 주문 번호와 메뉴 리스트를 텍스처로 변환
- 물리 시뮬레이션 적용으로 현실감 있는 출력 효과

```cpp
void AReceiptActor::OnUpdateCanvas(UCanvas* Canvas, int32 Width, int32 Height)
{
    // 배경 흰색
    FCanvasTileItem TileItem(FVector2D(0, 0), 
                             FVector2D(Width, Height), 
                             FLinearColor::White);
    Canvas->DrawItem(TileItem);
    
    // 주문 번호 크게 표시
    FString OrderText = FString::Printf(TEXT("#%d"), OrderNum);
    // ... 텍스트 렌더링
    
    // 메뉴 리스트 표시
    for (const FString& M : MenuList)
    {
        FCanvasTextItem MenuItem(...);
        Canvas->DrawItem(MenuItem);
    }
}
```

---

### 5.1 로딩 화면 시스템

#### LoadingWidget 클래스
- **목적**: 레벨 전환 시 표시되는 로딩 UI
- **구현 위치**: 
  - `Source/MHGA/Public/LoadingWidget.h`
  - `Source/MHGA/Private/LoadingWidget.cpp`
- **Blueprint**: `/Game/UI/WBP_LoadingUI`

#### LoadingBurger 클래스  
- **목적**: 로딩 화면에서 회전하는 3D 버거 표시
- **주요 컴포넌트**:
  - `SceneCaptureComponent2D`: 버거를 렌더 타겟에 캡처
  - `StaticMeshComponent`: 회전하는 버거 메쉬
  - `RotationSpeed`: 초당 90도 회전
- **구현 위치**: 
  - `Source/MHGA/Public/LoadingBurger.h`
  - `Source/MHGA/Private/LoadingBurger.cpp`

**로딩 화면 관리 (MHGAGameInstance)**
```cpp
// MHGAGameInstance.h
private:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class ULoadingWidget> LoadingWidgetClass;
    
    UPROPERTY()
    class ULoadingWidget* LoadingWidget;

public:
    void ShowLoadingScreen();
    void HideLoadingScreen();
```

**로딩 화면 표시/숨김 구현**
```cpp
void UMHGAGameInstance::ShowLoadingScreen()
{
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return;

    // Loading Widget 생성
    if (!LoadingWidget && LoadingWidgetClass)
    {
        LoadingWidget = CreateWidget<ULoadingWidget>(PC, LoadingWidgetClass);
    }

    // Widget 표시 (최상위 Z-Order)
    if (LoadingWidget)
    {
        LoadingWidget->AddToViewport(9999);
    }
}

void UMHGAGameInstance::HideLoadingScreen()
{
    // Loading Widget 제거
    if (LoadingWidget && LoadingWidget->IsInViewport())
    {
        LoadingWidget->RemoveFromParent();
    }

    // 입력 모드 복구
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false;
        }
    }
}
```

**PlayerController RPC**
```cpp
// 클라이언트에 로딩 표시 요청
UFUNCTION(Client, Reliable)
void ClientShowLoading();

// 클라이언트에 로딩 숨김 요청  
UFUNCTION(Client, Reliable)
void ClientHideLoading();
```

#### 사용 시점 및 흐름

1. **로비 → 메인 레벨 전환**
```cpp
void ULobbyUI::OnClickReady()
{
    if (LobbyBoard->HasAuthority())
    {
        // 모든 플레이어에게 로딩창 표시 요청
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (AMHGAPlayerController* PC = Cast<AMHGAPlayerController>(It->Get()))
            {
                PC->ClientShowLoading();
            }
        }

        // 약간의 지연 후 서버 트래블
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            GetWorld()->ServerTravel(TEXT("/Game/Maps/Main"), true);
        }, 0.1f, false);
    }
}
```

2. **메인 레벨 도착 후 로딩 숨김**
```cpp
void AMHGAGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (AMHGAPlayerController* PC = Cast<AMHGAPlayerController>(NewPlayer))
    {
        // 약간의 지연 후 로딩창 숨김
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [PC]()
        {
            if (PC)
            {
                PC->ClientHideLoading();
            }
        }, 3.0f, false); // 3초 후 숨김
    }
}
```

3. **게임 종료 → 로비 복귀**
```cpp
void AMHGAGameMode::HandleGameOver(FString reason)
{
    // ... 게임 오버 처리 ...
    
    if (HasAuthority())
    {
        FTimerHandle handle;
        GetWorldTimerManager().SetTimer(handle, [this]()
        {
            GetWorld()->ServerTravel(TEXT("/Game/Maps/Lobby"), true);
        }, 10, false);
    }
}
```

---

### 5.2 게임 오버 시스템

#### GameOverWidget 클래스
- **목적**: 게임 종료 시 점수 표시 및 결과 화면
- **구현 위치**: 
  - `Source/MHGA/Public/GameOverWidget.h`
  - `Source/MHGA/Private/GameOverWidget.cpp`
- **실제 구현**: `PlayerWidget` 클래스 내에 통합되어 있음
  - `finalScore` Canvas Panel
  - `orderScoreBox`, `cookScoreBox`, `foodScoreBox` TextBlock
  - `FinalScoreAnim` 애니메이션

#### MHGAGameState - 게임 상태 관리
- **평점 시스템**: 
  - `orderSpeedScore`: 주문 속도 평점
  - `cookSpeedScore`: 조리 속도 평점  
  - `foodScore`: 음식 정확도 평점
  - `finalScore`: 총 평점
- **게임 상태 플래그**:
  - `bIsGamePlaying`: 게임 진행 중 여부
  - `bIsGameOver`: 게임 종료 여부

**게임 오버 처리 흐름**
```cpp
// 1. 서버에서 게임 오버 감지 및 처리
void AMHGAGameMode::HandleGameOver(FString reason)
{
    AMHGAGameState* gs = GetGameState<AMHGAGameState>();
    if (gs && !gs->bIsGameOver)
    {
        gs->bIsGameOver = true;  // 플래그 설정 (RepNotify 발동)
        
        // 모든 손님 퇴장
        ACustomerManager* CustomerManager = Cast<ACustomerManager>(
            UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass())
        );
        if (CustomerManager)
        {
            CustomerManager->KickAllCustomers();
        }
        
        // 클라이언트에서 OnRep_GameOver 호출됨
        gs->OnRep_GameOver();
        
        // 10초 후 로비로 이동
        if (HasAuthority())
        {
            FTimerHandle handle;
            GetWorldTimerManager().SetTimer(handle, [this]()
            {
                GetWorld()->ServerTravel(TEXT("/Game/Maps/Lobby"), true);
            }, 10, false);
        }
    }
}

// 2. 클라이언트 게임 오버 처리
void AMHGAGameState::OnRep_GameOver()
{
    if (bIsGameOver)
    {
        // 최종 점수 UI 표시
        playerWidget->finalScore->SetVisibility(ESlateVisibility::Visible);
        playerWidget->PlayFinalScoreAnim();
        
        // 플레이어 컨트롤러에 게임 오버 처리 요청
        AMHGAPlayerController* pc = Cast<AMHGAPlayerController>(
            UGameplayStatics::GetPlayerController(GetWorld(), 0)
        );
        if (pc)
        {
            // 플레이어 입력 처리는 GameMode에서 처리
        }
    }
}
```

#### 평점 시스템

**평점 변경 이유 enum**
```cpp
UENUM(BlueprintType)
enum class EScoreChangeReason : uint8
{
    SlowOrder   UMETA(DisplayName="주문 지연"),
    SlowCook    UMETA(DisplayName="조리 지연"),
    WrongFood   UMETA(DisplayName="잘못된 음식"),
    CorrectFood UMETA(DisplayName="올바른 음식")
};
```

**점수 시스템 다이어그램**

```mermaid
graph TB
    subgraph "Game Events"
        E1["⏳ 주문 지연<br/>손님이 오래 대기"]
        E2["🍳 조리 지연<br/>음식이 늦게 나옴"]
        E3["❌ 잘못된 음식<br/>레시피 불일치"]
        E4["✅ 올바른 음식<br/>정확한 레시피"]
    end

    subgraph "Score Change"
        S1["📊 orderSpeedScore<br/>-5점"]
        S2["📊 cookSpeedScore<br/>-10점"]
        S3["📊 foodScore<br/>-15점"]
        S4["📊 foodScore<br/>+10점"]
    end

    subgraph "Final Score Calculation"
        TOTAL["🎯 finalScore<br/>= orderSpeed + cookSpeed + foodScore"]
    end

    subgraph "Rating"
        R1["⭐⭐⭐⭐ 넌 정직원 해라<br/>250점 이상"]
        R2["⭐⭐⭐ 잘하시네요<br/>150-249점"]
        R3["⭐⭐ 평균입니다<br/>100-149점"]
        R4["⭐ 분발하세요<br/>99점 이하"]
    end

    E1 --> S1
    E2 --> S2
    E3 --> S3
    E4 --> S4

    S1 --> TOTAL
    S2 --> TOTAL
    S3 --> TOTAL
    S4 --> TOTAL

    TOTAL --> R1
    TOTAL --> R2
    TOTAL --> R3
    TOTAL --> R4

    style E1 fill:#FFB6C1
    style E2 fill:#FFB6C1
    style E3 fill:#FFB6C1
    style E4 fill:#98FB98
    style S1 fill:#FFB6C1
    style S2 fill:#FFB6C1
    style S3 fill:#FFB6C1
    style S4 fill:#98FB98
    style TOTAL fill:#F0E68C
    style R1 fill:#FFD700
    style R2 fill:#C0C0C0
    style R3 fill:#CD7F32
    style R4 fill:#A9A9A9
```

**GameMode의 평점 설정 변수**
```cpp
// MHGAGameMode.h
UPROPERTY(EditAnywhere, Category = "Game Config")
int32 penaltySlowOrder = -5;      // 주문 지연 패널티
UPROPERTY(EditAnywhere, Category = "Game Config")
int32 penaltySlowCook = -10;      // 조리 지연 패널티
UPROPERTY(EditAnywhere, Category = "Game Config")
int32 penaltyWrongFood = -15;     // 잘못된 음식 패널티
UPROPERTY(EditAnywhere, Category = "Game Config")
int32 bonusCorrectFood = 10;      // 올바른 음식 보너스
```

**평점 변경 처리**
```cpp
void AMHGAGameMode::ReportScoreChanged(EScoreChangeReason reason, int32 changeScore)
{
    if (!HasAuthority()) return;
    AMHGAGameState* gs = GetGameState<AMHGAGameState>();
    if (!gs || gs->bIsGameOver) return;

    int32* targetScore = nullptr;

    switch (reason)
    {
    case EScoreChangeReason::SlowOrder:
        targetScore = &gs->orderSpeedScore;
        changeScore = penaltySlowOrder;
        break;
    case EScoreChangeReason::SlowCook:
        targetScore = &gs->cookSpeedScore;
        changeScore = penaltySlowCook;
        break;
    case EScoreChangeReason::WrongFood:
        targetScore = &gs->foodScore;
        changeScore = penaltyWrongFood;
        break;
    case EScoreChangeReason::CorrectFood:
        targetScore = &gs->foodScore;
        changeScore = bonusCorrectFood;
        break;
    }

    if (targetScore && changeScore != 0)
    {
        *targetScore += changeScore;
        gs->finalScore = gs->orderSpeedScore + gs->cookSpeedScore + gs->foodScore;
    }
}
```

**최종 평가 기준**
```cpp
FText AMHGAGameState::GetFinalRank()
{
    if (finalScore >= 250)
        return FText::FromString(TEXT("넌 정직원 해라"));
    else if (finalScore >= 150)
        return FText::FromString(TEXT("잘하시네요"));
    else if (finalScore >= 100)
        return FText::FromString(TEXT("평균입니다"));
    else
        return FText::FromString(TEXT("분발하세요"));
}
```

---

### 6. 상호작용 시스템

**InteractComponent를 통한 통합 상호작용 관리**

#### 시스템 개요

플레이어가 월드의 다양한 오브젝트와 상호작용할 수 있는 컴포넌트 기반 시스템

**지원 상호작용**
- 재료/도구 집기 (Grab)
- 재료 컨테이너에서 꺼내기
- 도구 사용 (Use)
- 오브젝트 내려놓기 (Put)

#### 상호작용 플로우

```mermaid
graph TD
    A["⌨️ E키 입력<br/>GrabProps"] --> B["🎯 Sweep<br/>Collision 감지"]
    B --> C{액터<br/>타입 확인}

    C -->|WrappingPaper| D["📦 TryWrap<br/>포장지 상호작용"]
    C -->|IGrabableProps| E["✋ MulticastRPC<br/>GrabProps 실행"]
    C -->|IngContainer| F["🔄 스폰<br/>새 재료 생성"]

    E --> G["💫 물리 비활성화<br/>Grab 상태"]
    G --> H["🎥 카메라에<br/>부착"]

    D --> I["✓ 레시피 매칭"]
    I --> J["🍔 햄버거 생성<br/>& 영수증"]

    F --> H

    H --> K["🖱️ 마우스<br/>사용/좌클릭"]
    K --> L{도구<br/>타입?}

    L -->|Flipper| M["⚡ Flip 동작<br/>패티 뒤집기"]
    L -->|SauceBottle| N["💦 Shoot<br/>소스 발사"]
    L -->|기타| O["🔧 일반 사용"]

    M --> P["E키 다시<br/>PutProps"]
    N --> P
    O --> P

    P --> Q["🪑 물리 활성화<br/>월드에 배치"]

    style A fill:#FFE4B5
    style B fill:#87CEEB
    style C fill:#DDA0DD
    style D fill:#F0E68C
    style E fill:#90EE90
    style F fill:#90EE90
    style G fill:#FFB6C1
    style H fill:#FFB6C1
    style I fill:#F0E68C
    style J fill:#98FB98
    style K fill:#FFE4B5
    style L fill:#DDA0DD
    style M fill:#FFA07A
    style N fill:#FFA07A
    style O fill:#FFA07A
    style P fill:#FFB6C1
    style Q fill:#90EE90
```

#### IGrabableProps 인터페이스

```cpp
class IGrabableProps
{
    virtual void OnGrabbed(AMHGACharacter* Player) = 0;  // 집힐 때
    virtual void OnPut() = 0;                             // 내려놓을 때
    virtual void OnUse() = 0;                             // 사용할 때
    virtual void SetLocation(FVector Loc) = 0;            // 위치 설정
};
```

```cpp
class AIngredientBase : public AActor, public IGrabableProps
{
    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;
    
    UPROPERTY(EditAnywhere)
    EIngredient IngType = EIngredient::None;
    
    UPROPERTY(EditAnywhere)
    AMHGACharacter* GrabCharacter;  // 현재 집고 있는 캐릭터
};
```

**인터페이스 구현 클래스**
- `AIngredientBase` 및 모든 하위 재료 클래스
- `AHamburger` (완성된 햄버거)
- `AFlipper` (패티 뒤집개, 자체 GrabCharacter 필드 보유)
- `AReceiptActor` (영수증)

#### 집기 시스템 구현

**1. Sweep 기반 탐지**
```cpp
void UInteractComponent::GrabProps()
{
    FVector Start = Owner->GetFirstPersonCameraComponent()->GetComponentLocation();
    FVector End = Start + Owner->GetCamera()->GetForwardVector() * GrabDistance;
    
    FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
    
    if (GetWorld()->SweepSingleByChannel(Hit, Start, End, ...))
    {
        // WrappingPaper 특수 처리
        if (AWrappingPaper* wp = Cast<AWrappingPaper>(Hit.GetActor()))
        {
            wp->TryWrap();  // 포장 시도
            return;
        }
        
        // IGrabableProps 인터페이스 체크
        if (Cast<IGrabableProps>(Hit.GetActor()))
            MulticastRPC_GrabProps(Hit);
            
        // IngContainer 특수 처리
        else if (AIngContainer* Container = Cast<AIngContainer>(Hit.GetActor()))
        {
            AIngredientBase* Ing = Container->GetIngredient();
            MulticastRPC_InteractIngContainer(Ing);
        }
    }
}
```

**2. 네트워크 동기화**
```cpp
void UInteractComponent::MulticastRPC_GrabProps_Implementation(FHitResult Hit)
{
    IGrabableProps* GrabInterface = Cast<IGrabableProps>(Hit.GetActor());
    GrabInterface->OnGrabbed(Owner);
    
    // 물리 끄기
    Hit.GetComponent()->SetSimulatePhysics(false);
    Hit.GetComponent()->SetCollisionProfileName(TEXT("Grabbed"));
    
    // 카메라에 부착
    HoldDistance = FVector::Dist(CameraLocation, ActorLocation);
    HoldDistance = FMath::Clamp(HoldDistance, 50, 200);
    
    Cast<AActor>(GrabInterface)->AttachToComponent(
        Owner->GetFirstPersonCameraComponent(), 
        FAttachmentTransformRules::SnapToTargetNotIncludingScale
    );
    
    GrabInterface->SetLocation(
        CameraLocation + CameraForward * HoldDistance
    );
    
    bIsGrabbed = true;
    GrabbedProp = GrabInterface;
}

// IngredientBase의 OnGrabbed - 다른 플레이어가 집고 있으면 처리
void AIngredientBase::OnGrabbed(AMHGACharacter* Player)
{
    if (GrabCharacter != Player && GrabCharacter != nullptr)
    {
        // 이전에 집고 있던 캐릭터가 있으면 내려놓게 함
        GrabCharacter->GetInteractComponent()->PutProps();
    }
    GrabCharacter = Player;
}
```

**3. 내려놓기**
```cpp
void UInteractComponent::PutProps()
{
    if (GrabbedProp)
    {
        GrabbedProp->OnPut();
        
        AActor* P = Cast<AActor>(GrabbedProp);
        if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(P->GetRootComponent()))
        {
            // 물리 다시 켜기
            Root->SetSimulatePhysics(true);
            Root->SetCollisionProfileName(TEXT("BlockAllDynamic"));
            P->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        }
        
        bIsGrabbed = false;
        GrabbedProp = nullptr;
    }
}
```

#### 특수 도구: Flipper (뒤집개)

**패티 뒤집기 구현**
```cpp
void AFlipper::OnUse()
{
    FVector Start = GrabCharacter->GetCamera()->GetComponentLocation();
    FVector End = Start + GrabCharacter->GetCamera()->GetForwardVector() * 200;
    
    if (GetWorld()->SweepSingleByChannel(Hit, Start, End, ...))
    {
        if (Hit.GetComponent()->IsSimulatingPhysics())
        {
            // 카메라의 Right 벡터 기준으로 180도 회전
            FRotationMatrix CamMatrix(CameraRotation);
            FVector RightVector = CamMatrix.GetScaledAxis(EAxis::Y);
            
            FQuat CurrentQuat = Hit.GetActor()->GetActorQuat();
            FQuat FlipQuat = FQuat(RightVector, FMath::DegreesToRadians(180.f));
            FQuat NewQuat = FlipQuat * CurrentQuat;
            
            Hit.GetActor()->SetActorRotation(NewQuat);
        }
    }
}
```

#### 재료 컨테이너 시스템

**AIngContainer 클래스**
- 무한 재료 공급 시스템
- Interact 시 새 재료 인스턴스 스폰

```cpp
class AIngContainer : public AActor
{
    UPROPERTY(EditAnywhere)
    TSubclassOf<AIngredientBase> Ingredient;
    
    AIngredientBase* GetIngredient()
    {
        return GetWorld()->SpawnActor<AIngredientBase>(Ingredient);
    }
};
```

---

### 7. 캐릭터 및 입력 시스템

**Enhanced Input System 기반의 1인칭 캐릭터 컨트롤**

#### MHGACharacter 구조

**컴포넌트 구성**
- **UCameraComponent**: 1인칭 시점 카메라
  - FirstPersonFieldOfView: 70도
  - FirstPersonScale: 0.6 (손 크기 조절)
- **UInteractComponent**: 오브젝트 상호작용
- **UWidgetInteractionComponent**: 3D UI 상호작용
- **USkeletalMeshComponent**: 1인칭 바디 메시

#### Enhanced Input Actions

```cpp
// 이동
IA_Move   : Vector2D (WASD)
// 시점
IA_Look   : Vector2D (마우스)
// 집기/놓기
IA_Pick   : Button (E)
// 사용
IA_Use    : Button (마우스 좌클릭)
// 앉기
IA_Crouch : Button (Ctrl)
// 게임 시작 (호스트만)
IA_Start  : Button (G) 
// 음성 채팅
IA_Voice  : Button (V - Hold)
// UI 모드 전환
IA_UI     : Button (Tab - Hold)
```

**입력 처리 예시**
```cpp
void AMHGACharacter::PickInput(const FInputActionValue& Value)
{
    if (IsLocallyControlled())
        InteractComponent->ServerRPC_InteractProps();
}

void AMHGACharacter::UseInput(const FInputActionValue& Value)
{
    if (IsLocallyControlled())
    {
        // 3D UI 상호작용
        WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
        
        // Props 사용
        InteractComponent->ServerRPC_UseProps();
    }
}

void AMHGACharacter::StartInput(const FInputActionValue& Value)
{
    if (HasAuthority())  // 서버(호스트)에서만 실행
    {
        AMHGAGameMode* gm = Cast<AMHGAGameMode>(UGameplayStatics::GetGameMode(this));
        AMHGAGameState* gs = Cast<AMHGAGameState>(UGameplayStatics::GetGameState(this));
        if (gm && gs && !gs->bIsGamePlaying)
        {
            gm->GameStart();  // 게임 시작
        }
    }
}

void AMHGACharacter::StartVoiceInput()
{
    AMHGAPlayerController* pc = GetController<AMHGAPlayerController>();
    pc->StartTalking();  // 음성 채팅 시작
}

void AMHGACharacter::StartUIInput()
{
    APlayerController* pc = GetWorld()->GetFirstPlayerController();
    UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(pc);
    pc->SetShowMouseCursor(true);  // 마우스 커서 표시
}
```

#### Widget Interaction 구성

**3D POS 시스템 상호작용**
```cpp
WidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(...);
WidgetInteraction->SetupAttachment(FPSCamComponent);
WidgetInteraction->InteractionDistance = 200.f;
WidgetInteraction->bShowDebug = true;  // 디버그 라인 표시
```

**동작 방식**
1. 카메라 중앙에서 Ray 발사
2. Widget Component와 충돌 시 클릭 이벤트 전달
3. 로컬 플레이어에서만 활성화

#### 애니메이션 시스템 (PlayerAnim)

**Blend Space 기반 이동 애니메이션**
```cpp
class UPlayerAnim : public UAnimInstance
{
    UPROPERTY(BlueprintReadOnly)
    float Speed = 0;      // 전진/후진 속도
    
    UPROPERTY(BlueprintReadOnly)
    float Dir = 0;        // 좌우 이동
    
    UPROPERTY(BlueprintReadOnly)
    float CameraRotX = 0; // 카메라 상하 회전
    
    UPROPERTY(BlueprintReadOnly)
    bool bIsCrouch = false;
};
```

**애니메이션 업데이트**
```cpp
void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
    if (OwnerPlayer)
    {
        // 전후 이동 속도 (정규화)
        Speed = FVector::DotProduct(
            OwnerPlayer->GetVelocity(), 
            OwnerPlayer->GetActorForwardVector()
        ) / OwnerPlayer->GetCharacterMovement()->GetMaxSpeed();
        
        // 좌우 이동 속도 (정규화)
        Dir = FVector::DotProduct(
            OwnerPlayer->GetVelocity(), 
            OwnerPlayer->GetActorRightVector()
        ) / OwnerPlayer->GetCharacterMovement()->GetMaxSpeed();
        
        // 카메라 피치
        CameraRotX = -OwnerPlayer->GetBaseAimRotation().Pitch;
        
        // 앉기 상태
        bIsCrouch = OwnerPlayer->GetCharacterMovement()->IsCrouching();
    }
}
```

---

## 시스템 아키텍처

```mermaid
graph TB
    subgraph "Network Layer"
        NET["🔄 Listen Server<br/>RPC & Replication"]
    end

    subgraph "Game Logic"
        AI["🤖 FSM-based AI<br/>Customer System"]
        COOK["🍳 Cooking System<br/>State Machine"]
        POS["💳 POS System<br/>Order Management"]
    end

    subgraph "Player Interaction"
        INTERACT["✋ Interact Component<br/>Physics-based Grab"]
        INPUT["⌨️ Enhanced Input<br/>Action-based Control"]
    end

    subgraph "Data Management"
        DATATABLE["📊 DataTable<br/>Recipe System"]
        GAMESTATE["📈 GameState<br/>Score Management"]
    end

    NET --> AI
    NET --> COOK
    NET --> POS
    INPUT --> INTERACT
    DATATABLE --> COOK
    DATATABLE --> POS
    GAMESTATE --> POS
    INTERACT --> COOK

    style NET fill:#87CEEB
    style AI fill:#DDA0DD
    style COOK fill:#F0E68C
    style POS fill:#FFB6C1
    style INTERACT fill:#98FB98
    style INPUT fill:#90EE90
    style DATATABLE fill:#FFE4B5
    style GAMESTATE fill:#FFE4B5
```

## 주요 기술 포인트

| 카테고리 | 기술 | 특징 |
|---------|------|------|
| **네트워크** | Listen Server + RPC | 멀티플레이어 동기화 |
| **AI** | FSM (9가지 상태) | 지능형 손님 행동 |
| **물리** | Physics-based Grab | 실감있는 상호작용 |
| **UI** | Widget Component 3D | 몰입형 POS 시스템 |
| **데이터** | DataTable 기반 | 확장 가능한 레시피 관리 |
| **게임플레이** | 상태 머신 조리 | 타이밍 기반 게임 메커니즘 |

---

## 🎯 Tech Highlights

### 1. 데이터 주도 설계 (Data-Driven Design)

**DataTable을 활용한 레시피 관리**

```cpp
// BurgerData.h
USTRUCT(BlueprintType)
struct FBurgerRecipe : public FTableRowBase
{
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBurgerMenu BurgerName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FIngredientStack> Ingredients;
};
```

**장점**
- 프로그래머 없이 디자이너가 메뉴 추가/수정 가능
- 확장성 (신규 메뉴 추가 용이)

### 2. 네트워크 설계

**RPC 호출 체인 최적화**
```
Client Input 
→ ServerRPC (PlayerController) 
→ MulticastRPC (Counter Actor) 
→ All Clients Update UI
```

**이점**
- 서버에서 데이터 검증
- 모든 클라이언트 자동 동기화
- 클라이언트 간 불일치 방지

### 3. FSM 기반 AI 설계

**상태 전환 로직**
```cpp
void UCustomerFSM::SetState(EAIState NewState)
{
    if (CurrentState == NewState) return;
    
    StopWandering();
    CurrentState = NewState;
    
    switch (CurrentState)
    {
    case EAIState::GoingToLine:
        MoveToTarget(orderTarget);
        break;
    case EAIState::Ordering:
        StartOrder();
        break;
    // ...
    }
}
```

**장점**
- 디버깅 용이
- 상태 추가/수정 간단
- 명확한 행동 패턴

### 4. 컴포넌트 기반 아키텍처

**재사용 가능한 컴포넌트 설계**
- InteractComponent: 모든 상호작용 통합 관리
- CustomerFSM: AI 행동 로직 분리
- WidgetInteraction: 3D UI 상호작용

### 5. 물리 기반 게임플레이

**리얼리즘을 위한 물리 시뮬레이션**
- 모든 재료는 실제 물리 법칙 적용
- 집기/놓기 시 물리 on/off 전환
- 영수증, 햄버거 등 완성품도 물리 적용

---

## 🔧 사용 기술 스택

### 엔진 및 언어

```mermaid
graph LR
    A["Unreal Engine 5.6"] --> B["C++<br/>핵심 로직"]
    A --> C["Blueprint<br/>UI/Level"]

    style A fill:#000000,color:#fff,stroke:#FFD700,stroke-width:3px
    style B fill:#87CEEB
    style C fill:#DDA0DD
```

### Unreal Engine 모듈 (Dependencies)

```mermaid
graph TD
    subgraph "Foundation"
        F1["Core"]
        F2["CoreUObject"]
        F3["Engine"]
    end

    subgraph "Input & Control"
        I1["InputCore"]
        I2["EnhancedInput<br/>Input Actions"]
    end

    subgraph "AI & Navigation"
        A1["AIModule"]
        A2["NavigationSystem<br/>NavMesh"]
        A3["StateTreeModule"]
    end

    subgraph "UI & Graphics"
        U1["UMG<br/>Widget System"]
        U2["Slate<br/>Framework"]
    end

    F1 --> F2
    F2 --> F3
    F3 --> I1
    I1 --> I2
    F3 --> A1
    A1 --> A2
    A2 --> A3
    F3 --> U1
    U1 --> U2

    style F1 fill:#90EE90
    style F2 fill:#90EE90
    style F3 fill:#90EE90
    style I1 fill:#87CEEB
    style I2 fill:#87CEEB
    style A1 fill:#FFB6C1
    style A2 fill:#FFB6C1
    style A3 fill:#FFB6C1
    style U1 fill:#FFE4B5
    style U2 fill:#FFE4B5
```

**주요 모듈별 역할**

| 모듈 | 용도 |
|------|------|
| **EnhancedInput** | Input Action 기반 입력 처리, 상황별 입력 전환 |
| **AIModule** | AI 이동 및 행동 제어 |
| **NavigationSystem** | NavMesh 기반 경로 탐색, 배회 구현 |
| **StateTreeModule** | 상태 머신 및 행동 트리 |
| **UMG** | 3D Widget Component, 주문 UI, 손님 말풍선 |
| **Slate** | 로우레벨 UI 프레임워크 |

### 핵심 기술 적용

```mermaid
graph TB
    subgraph "Network"
        N1["Listen Server"]
        N2["RPC System"]
        N3["Replication"]
    end

    subgraph "AI & Behavior"
        AI1["FSM<br/>9개 상태"]
        AI2["NavMesh 이동"]
        AI3["타이머 기반<br/>배회"]
    end

    subgraph "Physics & Interaction"
        P1["Physics Simulation"]
        P2["Sweep Collision"]
        P3["Grab/Release"]
    end

    subgraph "Data Driven"
        D1["DataTable<br/>레시피"]
        D2["Configuration"]
        D3["Dynamic Loading"]
    end

    N1 --> N2
    N2 --> N3
    AI1 --> AI2
    AI2 --> AI3
    P1 --> P2
    P2 --> P3
    D1 --> D2
    D2 --> D3

    N1 -.-> AI1
    N1 -.-> P1
    N1 -.-> D1

    style N1 fill:#87CEEB,stroke:#0066cc,stroke-width:2px
    style N2 fill:#87CEEB
    style N3 fill:#87CEEB
    style AI1 fill:#DDA0DD,stroke:#990099,stroke-width:2px
    style AI2 fill:#DDA0DD
    style AI3 fill:#DDA0DD
    style P1 fill:#98FB98,stroke:#006600,stroke-width:2px
    style P2 fill:#98FB98
    style P3 fill:#98FB98
    style D1 fill:#FFE4B5,stroke:#FF6600,stroke-width:2px
    style D2 fill:#FFE4B5
    style D3 fill:#FFE4B5
```

---

## 🎮 게임 플로우 및 시나리오

### 전체 게임 사이클

```mermaid
sequenceDiagram
    participant P1 as 플레이어 1<br/>Host
    participant P2 as 플레이어 2<br/>Client
    participant Server as GameMode<br/>Server Logic
    participant Customer as AI 손님
    participant UI as POS UI

    Note over P1,P2: [게임 시작]
    P1->>Server: G키 누름 (GameStart)
    Server->>Server: 타이머 시작 (5분)
    Server->>Customer: 손님 스폰 시작

    Note over P1,P2: [주문 접수]
    Customer->>UI: 카운터 도착
    P1->>UI: 메뉴 선택
    UI->>Server: ServerRPC_AddMenu
    Server->>UI: MulticastRPC 동기화
    P1->>UI: 주문 버튼

    Note over P1,P2: [조리]
    P1->>Server: 재료 집기
    P1->>Server: 조리 영역에 배치
    Server->>Server: 조리 타이머<br/>0s→3s→8s

    Note over P1,P2: [포장]
    P2->>UI: 포장지에 재료 배치
    UI->>Server: 레시피 매칭
    Server->>Server: 햄버거 생성

    Note over P1,P2: [픽업]
    Customer->>UI: 픽업 존으로 이동
    Customer->>Server: 음식 검증
    Server->>Server: 점수 업데이트
    Customer->>Customer: 퇴장

    Note over P1,P2: [게임 종료]
    Server->>Server: 타임 오버
    Server->>UI: 최종 점수 표시
    Server->>P1: 로비로 이동
    Server->>P2: 로비로 이동
```

### 게임 상태 머신

```mermaid
graph TD
    A["🏠 로비<br/>Lobby"] -->|Ready 클릭| B["⏳ 로딩<br/>Loading"]
    B -->|레벨 로드 완료| C["🎮 게임 플레이<br/>Playing"]
    C -->|타이머 종료| D["📊 게임 오버<br/>GameOver"]
    D -->|10초 대기| A

    C -.->|일시정지 ESC| C

    style A fill:#90EE90
    style B fill:#87CEEB
    style C fill:#FFE4B5
    style D fill:#FFB6C1
```

---

## 📊 성능 최적화

### 1. 네트워크 최적화
- **Authority 체크**: 서버에서만 실행되어야 하는 로직 분리
- **선택적 복제**: 필요한 데이터만 복제
- **RPC 최소화**: 여러 클라이언트 호출을 하나의 Multicast로 통합

### 2. AI 최적화
- **타이머 기반 업데이트**: Tick 대신 타이머로 배회 로직 처리
- **NavMesh 캐싱**: 이동 경로 재계산 최소화

### 3. 물리 최적화
- **Collision Profile 활용**: 상황에 맞는 충돌 설정
- **Physics Sleep**: 정지 시 물리 시뮬레이션 중지

---

## 🚀 향후 개선 방향

### 계획된 기능
- [x] **평판 시스템**: 손님 만족도에 따른 점수 관리 (ShowScoreFeedback 함수로 구현됨)
- [x] **난이도 조절**: 손님 스폰 빈도, 주문 복잡도 조절 (MaxSpawnedCustomers 변수 존재)
- [ ] **추가 메뉴**: 사이드 메뉴, 음료 시스템
- [ ] **시간 제한 모드**: 제한 시간 내 최대한 많은 주문 처리

---

## 📝 코드 구조

```
Source/MHGA/
├── Public/
│   ├── AI/
│   │   ├── CustomerAI.h          # 손님 캐릭터
│   │   ├── CustomerFSM.h         # 손님 FSM
│   │   ├── CustomerManager.h    # 손님 생성/관리
│   │   └── CustomerUI.h          # 손님 UI
│   ├── Counter/
│   │   ├── CounterPOS.h          # POS 시스템
│   │   ├── CounterUI.h           # POS UI
│   │   ├── MenuButtonUI.h        # 메뉴 버튼
│   │   ├── CustomerButtonUI.h   # 주문 버튼
│   │   ├── PickupZone.h          # 픽업 구역
│   │   └── ReceiptActor.h        # 영수증
│   ├── Ingredient/
│   │   ├── IngredientBase.h      # 재료 베이스 클래스
│   │   ├── Patty.h               # 패티 (조리 상태)
│   │   ├── SauceBottle.h         # 소스 병
│   │   └── [기타 재료들]
│   ├── Player/
│   │   ├── MHGACharacter.h       # 플레이어 캐릭터
│   │   ├── MHGAPlayerController.h# 플레이어 컨트롤러
│   │   ├── InteractComponent.h   # 상호작용 컴포넌트
│   │   └── PlayerAnim.h          # 애니메이션
│   ├── Props/
│   │   ├── Flipper.h             # 뒤집개
│   │   └── IngContainer.h        # 재료 컨테이너
│   ├── BurgerData.h              # 데이터 정의
│   ├── CookingArea.h             # 조리 영역
│   ├── GrabableProps.h           # 상호작용 인터페이스
│   ├── Hamburger.h               # 완성된 햄버거
│   ├── WrappingPaper.h           # 포장지
│   ├── MHGAGameMode.h            # 게임 모드
│   └── MHGAGameState.h           # 게임 스테이트
└── Private/
    └── [대응되는 .cpp 파일들]
```

---

## Members
- 김홍범
- 황규환
- 허지웅


---

## 📜 라이선스

Copyright Epic Games, Inc. All Rights Reserved.

---

### 주요 성과

| 영역 | 성과 | 기술 |
|------|------|------|
| **네트워크** | Listen Server 멀티플레이 구현 | RPC Chain, Replication |
| **AI** | 9가지 상태의 FSM 기반 손님 AI | NavMesh, State Management |
| **게임플레이** | 물리 기반 상호작용 시스템 | Physics Grab/Release, Collision |
| **UI/UX** | 3D 몰입형 POS 시스템 | Widget Component, Canvas |
| **데이터** | 확장 가능한 레시피 관리 | DataTable, JSON 설정 |
| **성능** | 최적화된 네트워크 설계 | Authority Check, Selective Replication |



**마지막 업데이트**: 2025년 11월 8일
