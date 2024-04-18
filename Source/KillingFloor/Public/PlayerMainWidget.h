// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class KILLINGFLOOR_API UPlayerMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInfo", meta = (BindWidget))
	class UTextBlock* currentEnemyLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInfo", meta = (BindWidget))
	class UTextBlock* currentWave;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInfo", meta = (BindWidget))
	class UTextBlock* lastWave;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInfo", meta = (BindWidget))
	class UTextBlock* playerHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInfo", meta = (BindWidget))
	class UTextBlock* playerMoney;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInfo", meta = (BindWidget))
	class UImage* waveImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInfo", meta = (BindWidget))
	class USlider* shopDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInfo", meta = (BindWidget))
	class UTextBlock* gunCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInfo", meta = (BindWidget))
	class UTextBlock* earnMoney;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInfo", meta = (BindWidget))
	class UTextBlock* shopDistance;

	UPROPERTY(EditAnywhere, Category = "MoneyAnim", meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* EarnMoneyAnimation;

	virtual void NativeConstruct() override;

	UPROPERTY()
	class AKillingFloorGameModeBase* gm;
	UPROPERTY()
	class AVRPlayer* player;

	FTimerHandle moneyTimerHandle;
	bool isPrintMoney = false;
	float curMoney = 0;

	void PrintCurrentEnemyLeft();
	void PrintRemainingTime();
	void PrintCurrentWave();
	void PrintLastWave();
	UFUNCTION(BlueprintCallable)
	void PrintPlayerHP();
	UFUNCTION(BlueprintCallable)
	void PrintPlayerMoney();
	UFUNCTION(BlueprintCallable)
	void PrintPlayerMoneyAnimation(int32 prevMoney, int nextMoney);
	void PrintWaveImage(UTexture2D* waveTexture);
	void PrintShopDirection();
	void PrintGunCount();
	void PrintEarnMoney(int32 money);
	void PrintShopDistance();
};
