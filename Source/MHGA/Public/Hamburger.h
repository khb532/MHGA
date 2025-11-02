#pragma once

#include "CoreMinimal.h"
#include "GrabableProps.h"
#include "GameFramework/Actor.h"
#include "Hamburger.generated.h"

UCLASS()
class MHGA_API AHamburger : public AActor, public IGrabableProps
{
	GENERATED_BODY()

/* Method */
public:
	AHamburger();

	void OnGrabbed(AMHGACharacter* Player) override {}
	void OnPut() override	{}
	void OnUse() override	{}
	virtual void SetLocation(FVector Loc) override;
	virtual UStaticMeshComponent* GetMeshComp() override	{ return Mesh;}
	virtual bool IsTool() override { return false; }
	
	UFUNCTION(Server, Reliable)
	void ServerSetName(const FString& Name);
	
	void SetName(FString Name);
	void SetMat(FString Name);
	// 버거 외형 텍스처
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> frontMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> wrongTexture;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> shrimpTexture;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> bigMacTexture;

	FString GetBurgerName();

	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	
private:
	void PrintLog();



/* Field */
public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere)
	bool bShowLog = true;

	
private:
	UPROPERTY(Replicated)
	FString BurgerName;
	
};
