// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KillingFloorGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class KILLINGFLOOR_API AKillingFloorGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKillingFloorGameModeBase();

public:
	UPROPERTY(BlueprintReadWrite)
	class UPlayerMainWidget* main_UI;

	UPROPERTY(BlueprintReadWrite)
	class AVRPlayer* player;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> endWinWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> endLoseWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> playerHitWidget;

	UPROPERTY(EditAnywhere)
	class USoundBase* countDownSound;
	UPROPERTY(EditAnywhere)
	class USoundBase* waveStartSound;
	UPROPERTY(EditAnywhere)
	class USoundBase* battleBackgroundSound;
	UPROPERTY(EditAnywhere)
	class USoundBase* shopBackgroundSound;

	UPROPERTY()
	class UAudioComponent* backgroundSoundComp;

	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetCurrentEnemyLeft() { return currentEnemyLeft; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetCurrentWave() { return currentWave; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetLastWave() { return LastWave; }

	UFUNCTION(BlueprintCallable)
	void AddCurrentEnemyLeft();
	UFUNCTION(BlueprintCallable)
	void SubtractCurrentEnemyLeft();

	void AddWave();

	UFUNCTION(BlueprintCallable)
	void CheckEnemyLeft();

	void RespawnEnemy();
	void SpawnEnemies();
	void SpawnBoss();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayBossMovie();

	void CountDown();

	void ShowVictoryWidget();
	UFUNCTION(BlueprintCallable)
	void ShowDefeatWidget();
	UFUNCTION(BlueprintCallable)
	void ShowPlayerHitWidget();
	void RemovePlayerHitWidget();

	void OpenStartLevel();

	void FindShop();


	// 적 처치시 확률적으로 제드 타임 발동
	void PlayZedTime(float playRate);
	void PlayOriginalTime();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int countDownSeconds = 20;

	int32 Seconds = 20;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABoss> boss;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> shopNavigation;

	UPROPERTY()
	AActor* shop;

	FTimerHandle TimerHandle;
	FTimerHandle countDownTimerHandle;
	FTimerHandle zedTimeHandle;
	FTimerHandle widgetAlphaTimer;
	FTimerHandle zedWidgetAlphaTimer;
	FTimerHandle spawnTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float alphaSpeed = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float widgetAlphaSpeed = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float zedWidgetAlphaSpeed = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float rotSpeed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isShowHitUI = false;
	bool isShowDefeatUI = false;
	bool isShowVictoryUI = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 currentWave = 0;

private:
	int32 currentEnemyLeft = 0;

	int32 LastWave = 3;

	float alpha = 0;
	float widgetAlpha = 0;
	float zedWidgetAlpha = 0;
	float easingDuration = 0;
	float zedEasingDuration = 0;
	float beforeRotAngle = 0;
	float rotAngle = 0;
	

	UPROPERTY()
	TArray<AActor*> enemySpawner;

	UPROPERTY()
	class UTexture2D* waveTexture;
	UPROPERTY()
	class UTexture2D* clockTexture;

	UPROPERTY()
	class AActor* shopNavigationGuide;

	UPROPERTY()
	class UUserWidget* winEnd_UI;
	UPROPERTY()
	class UUserWidget* loseEnd_UI;
	UPROPERTY()
	class UUserWidget* playerHit_UI;
	UPROPERTY()
	class UEndWidget* defeatUI;

	UPROPERTY()
	class UMaterialParameterCollection* matCollection;
	UPROPERTY()
	class UMaterialParameterCollectionInstance* pcimatCollection;

	float EaseInQuint(float start, float end, float value);
};
