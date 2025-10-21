#pragma once

#include "CoreMinimal.h"
#include "BurgerData.h"
#include "Blueprint/UserWidget.h"
#include "CustomerButtonUI.generated.h"

class ACustomerAI;
class UCounterUI;
class UTextBlock;
class UButton;

UCLASS()
class MHGA_API UCustomerButtonUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UCounterUI* CounterUI;

	UPROPERTY()
	ACustomerAI* Customer;
	
	UPROPERTY()
	int32 Num;
	UPROPERTY()
	TArray<EBurgerMenu> Menus;

	
protected:
	UPROPERTY(meta=(BindWidget))
	UButton* BTN_Customer;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TEXT_CustomerNum;

protected:
	UFUNCTION(BlueprintCallable)
	void OnClickedBtn();
	
public:
	void Init(TArray<EBurgerMenu> InMenu, int32 InNum, UCounterUI* InOwner);
	int32 GetNum();
	TArray<EBurgerMenu>& GetMenuInfo();
	void SetCustomer(ACustomerAI* InCustomer);
	ACustomerAI* GetCustomer() {return Customer;}
};
