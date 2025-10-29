// Fill out your copyright notice in the Description page of Project Settings.


#include "Counter/ReceiptActor.h"

#include "MHGA.h"
#include "Components/BoxComponent.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"

// Sets default values
AReceiptActor::AReceiptActor()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetSimulatePhysics(true);
	BoxComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	BoxComponent->SetBoxExtent(FVector(16,26,0.5f));

	PaperMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaperMesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> sm(TEXT("/Script/Engine.StaticMesh'/Game/Blueprints/Counter/ReceiptMesh.ReceiptMesh'"));
	if (sm.Succeeded())
		PaperMesh->SetStaticMesh(sm.Object);
	ConstructorHelpers::FObjectFinder<UMaterial> mat(TEXT("/Script/Engine.Material'/Game/Blueprints/Counter/M_Receipt.M_Receipt'"));
	if (mat.Succeeded())
		PaperMesh->SetMaterial(0, mat.Object);
	PaperMesh->SetupAttachment(RootComponent);
	PaperMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	bReplicates = true;
	SetReplicateMovement(true);
}

void AReceiptActor::BeginPlay()
{
	Super::BeginPlay();
}

void AReceiptActor::OnUpdateCanvas(UCanvas* Canvas, int32 Width, int32 Height)
{
	if (!Canvas) return;

	FLinearColor TextColor = FColor::Black;
	
	// 배경 흰색
	FCanvasTileItem TileItem(FVector2D(0, 0), FVector2D(Width, Height), FLinearColor::White);
	Canvas->DrawItem(TileItem);

	// 주문번호 (크게 중앙)
	UFont* NumFont = GEngine->GetLargeFont();
	FString OrderText = FString::Printf(TEXT("#%d"), OrderNum);

	FVector2D TextSize;
	Canvas->StrLen(NumFont, OrderText, TextSize.X, TextSize.Y);

	float Scale = 5.0f;
	float CenterX = 70;
	float TopY = 60.0f;

	FCanvasTextItem OrderItem(FVector2D(CenterX, TopY), FText::FromString(OrderText), NumFont, TextColor);
	OrderItem.Scale = FVector2D(Scale, Scale);
	OrderItem.bOutlined = true;
	OrderItem.OutlineColor = FLinearColor::Black;
	Canvas->DrawItem(OrderItem);

	// 메뉴 리스트 (왼쪽 정렬)
	UFont* MenuFont = GEngine->GetSmallFont();
	float Y = TopY + TextSize.Y * Scale + 30.f;
	float LineHeight = 30.f;
	float LeftPadding = 25.f;

	for (const FString& M : MenuList)
	{
		FCanvasTextItem MenuItem(FVector2D(LeftPadding, Y), FText::FromString(M), MenuFont, TextColor);
		MenuItem.Scale = FVector2D(2, 2);
		MenuItem.bOutlined = true;
		MenuItem.OutlineColor = FLinearColor::Black;
		Canvas->DrawItem(MenuItem);
		Y += LineHeight;
	}
}

void AReceiptActor::SetLocation(FVector Loc)
{
	SetActorLocation(Loc);
}

void AReceiptActor::MulticastRPC_Init_Implementation(int32 InOrderNum, const TArray<FString>& InMenus)
{
	OrderNum = InOrderNum;
	MenuList = InMenus;
	
	RenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(GetWorld(), UCanvasRenderTarget2D::StaticClass(), 512, 512);

	// 머티리얼 인스턴스 생성
	DynamicMat = PaperMesh->CreateDynamicMaterialInstance(0);
	DynamicMat->SetTextureParameterValue(TEXT("ReceiptTexture"), RenderTarget);
	RenderTarget->OnCanvasRenderTargetUpdate.AddDynamic(this, &AReceiptActor::OnUpdateCanvas);
	
	// 즉시 업데이트
	RenderTarget->UpdateResource();
	PRINTINFO();
}
