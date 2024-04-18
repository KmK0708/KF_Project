// Fill out your copyright notice in the Description page of Project Settings.


#include "KillingFloorGameModeBase.h"
#include "PlayerMainWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EnemySpawner.h"
#include "VRPlayer.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "Boss.h"
#include "EndWidget.h"
#include "Components/Border.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

AKillingFloorGameModeBase::AKillingFloorGameModeBase()
{
	ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> tempMatCollection(TEXT("/Script/Engine.MaterialParameterCollection'/Game/Blueprints/MPC_AlphaCollection.MPC_AlphaCollection'"));
	if (tempMatCollection.Succeeded())
	{
		matCollection = tempMatCollection.Object;
	}
}

void AKillingFloorGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	waveTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Script/Engine.Texture2D'/Game/Assets/UI/Player/WaveImage4.WaveImage4'")));
	clockTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Script/Engine.Texture2D'/Game/Assets/UI/wall-clock.wall-clock'")));

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), enemySpawner);

	FindShop();

	CheckEnemyLeft();
}

float AKillingFloorGameModeBase::EaseInQuint(float start, float end, float value)
{
	/*end -= start;
	return end * value * value * value * value * value + start;*/

	value = FMath::Clamp(value, 0.0f, 1.0f);
	value = (FMath::Sin(value * PI * (0.2f + 2.5f * value * value * value)) * FMath::Pow(1.0f - value, 2.2f) + value) * (1.0f + (1.2f * (1.0f - value)));
	return start + (end - start) * value;
}

void AKillingFloorGameModeBase::AddCurrentEnemyLeft()
{
	currentEnemyLeft++;

	if (main_UI != nullptr)
	{
		main_UI->PrintCurrentEnemyLeft();
	}
}

void AKillingFloorGameModeBase::SubtractCurrentEnemyLeft()
{
	currentEnemyLeft--;

	if (main_UI != nullptr)
	{
		main_UI->PrintCurrentEnemyLeft();
	}
}

void AKillingFloorGameModeBase::AddWave()
{
	currentWave += 1;

	if (main_UI != nullptr)
	{
		if (currentWave != 3)
		{
			backgroundSoundComp->SetActive(false);
			UGameplayStatics::PlaySound2D(GetWorld(), waveStartSound);
			main_UI->PrintCurrentWave();
			player->waveStartUI->SetVisibility(true);
		}
		
	}
}

void AKillingFloorGameModeBase::CheckEnemyLeft()
{
	if (currentEnemyLeft == 0)
	{
		Seconds = countDownSeconds;
		GetWorldTimerManager().SetTimer(countDownTimerHandle, this, &AKillingFloorGameModeBase::CountDown, 1.0f, true, 0);

		if (currentWave != 0 && currentWave != 3)
		{
			player->waveEndUI->SetVisibility(true);
		}
		if (backgroundSoundComp)
		{
			backgroundSoundComp->SetActive(false);
		}
		backgroundSoundComp = UGameplayStatics::SpawnSound2D(GetWorld(), shopBackgroundSound);

		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		if (shopNavigation)
		{
			//shopNavigationGuide = GetWorld()->SpawnActor<AActor>(shopNavigation, FVector(-1958, 86, 69), FRotator(0, 0, 0), param);
			shopNavigationGuide = GetWorld()->SpawnActor<AActor>(shopNavigation, shop->GetActorLocation() + FVector(300, 0, -50), FRotator(0, 0, 0), param);
		}

		// 이미지를 시계로 바꾼다.
		if (main_UI)
		{
			main_UI->PrintWaveImage(clockTexture);
		}
	}
}

void AKillingFloorGameModeBase::RespawnEnemy()
{
	if (main_UI != nullptr)
	{
		main_UI->PrintWaveImage(waveTexture);
		main_UI->PrintCurrentEnemyLeft();
	}

	if (currentWave == LastWave)
	{
		PlayBossMovie();
		GetWorldTimerManager().SetTimer(spawnTimer, this, &AKillingFloorGameModeBase::SpawnBoss, 8.0f);
		
		return;
	}

	
	GetWorldTimerManager().SetTimer(spawnTimer, this, &AKillingFloorGameModeBase::SpawnEnemies, 5.0f);
}

void AKillingFloorGameModeBase::SpawnEnemies()
{
	backgroundSoundComp = UGameplayStatics::SpawnSound2D(GetWorld(), battleBackgroundSound);
	player->waveStartUI->SetVisibility(false);
	for (int32 i = 0; i < enemySpawner.Num(); i++)
	{
		Cast<AEnemySpawner>(enemySpawner[i])->SpawnEnemy();
	}
}

void AKillingFloorGameModeBase::SpawnBoss()
{
	//player->waveStartUI->SetVisibility(false);
	backgroundSoundComp = UGameplayStatics::SpawnSound2D(GetWorld(), shopBackgroundSound);
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	GetWorld()->SpawnActor<ABoss>(boss, FVector(0, 0, 0), FRotator(0, 0, 0), param);
}

void AKillingFloorGameModeBase::CountDown()
{
	if ((currentWave != 0) && (countDownSeconds - Seconds == 5))
	{
		player->waveEndUI->SetVisibility(false);
	}

	if (Seconds != 0)
	{
		Seconds -= 1;
	}
	else
	{
		// 타이머 종료
		GetWorldTimerManager().ClearTimer(countDownTimerHandle);
		Seconds = countDownSeconds;
		
		AddWave();
		RespawnEnemy();
		if (shopNavigationGuide)
		{
			shopNavigationGuide->Destroy();
		}
		return;
	}

	if (main_UI != nullptr)
	{
		if (Seconds < 10)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), countDownSound);
		}
		main_UI->PrintRemainingTime();
	}
}

void AKillingFloorGameModeBase::ShowVictoryWidget()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);

	if (isShowVictoryUI)
	{
		return;
	}

	if (backgroundSoundComp)
	{
		backgroundSoundComp->SetActive(false);
	}

	FInputModeUIOnly inputMode;
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	GetWorld()->GetFirstPlayerController()->SetInputMode(inputMode);

	//winEnd_UI = CreateWidget<UUserWidget>(GetWorld(), endWinWidget);

	//if (winEnd_UI != nullptr)
	//{
	//	winEnd_UI->AddToViewport(100);
	//}

	isShowVictoryUI = true;
	player->victoryEndUI->SetVisibility(true);

	FTimerHandle endTimerHandle;
	GetWorldTimerManager().SetTimer(endTimerHandle, this, &AKillingFloorGameModeBase::OpenStartLevel, 5.0f);
	
}

void AKillingFloorGameModeBase::ShowDefeatWidget()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	if (isShowDefeatUI)
	{
		return;
	}

	if (backgroundSoundComp)
	{
		backgroundSoundComp->SetActive(false);
	}

	FInputModeUIOnly inputMode;
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	GetWorld()->GetFirstPlayerController()->SetInputMode(inputMode);

	//loseEnd_UI = CreateWidget<UUserWidget>(GetWorld(), endLoseWidget);

	//if (loseEnd_UI != nullptr)
	//{
	//	loseEnd_UI->AddToViewport(100);
	//}

	isShowDefeatUI = true;
	player->defeatEndUI->SetVisibility(true);
	defeatUI = Cast<UEndWidget>(player->defeatEndUI->GetUserWidgetObject());


	player->backgroundColor->SetVisibility(true);

	pcimatCollection = GetWorld()->GetParameterCollectionInstance(matCollection);
	if (pcimatCollection)
	{
		widgetAlpha = 0;
		easingDuration = 0;
		//player->VRCamera->bUsePawnControlRotation = true;
		GetWorldTimerManager().SetTimer(widgetAlphaTimer, FTimerDelegate::CreateLambda(
			[this]()->void
			{

				widgetAlpha += widgetAlphaSpeed * GetWorld()->DeltaTimeSeconds;
				easingDuration += GetWorld()->DeltaTimeSeconds;

				//float easingAlpha = widgetAlpha * widgetAlpha * widgetAlpha * widgetAlpha;
				float easingAlpha = 1 - FMath::Pow(2, -6 * widgetAlpha) * FMath::Abs(FMath::Cos(widgetAlpha * PI * 3.5));
				 
				pcimatCollection->SetScalarParameterValue(FName("BackgroundAlphaScalar"), easingAlpha);

				rotAngle += rotSpeed * GetWorld()->DeltaTimeSeconds;
				//float rAngle = rotAngle * (2 - rotAngle);
				float rAngle = rotAngle * rotAngle * (2.70158 * rotAngle - 1.70158);
				
				float deltaRotAngle = rAngle - beforeRotAngle;
				beforeRotAngle = rAngle;
				
				//player->AddControllerPitchInput(-deltaRotAngle);
				//player->VRCamera->AddWorldRotation(FRotator(-deltaRotAngle * 10, 0, 0));
				player->VRCamera->AddLocalRotation(FRotator(-deltaRotAngle * 10, 0, 0));
				//player->AddActorWorldRotation(FRotator(-deltaRotAngle * 10, 0, 0));

				if (easingDuration > 5.0f)
				{
					pcimatCollection->SetScalarParameterValue(FName("BackgroundAlphaScalar"), 0.9f);
					GetWorldTimerManager().ClearTimer(widgetAlphaTimer);
				}
			}
		), 0.02f, true);

	}


	FTimerHandle endTimerHandle;
	GetWorldTimerManager().SetTimer(endTimerHandle, this, &AKillingFloorGameModeBase::OpenStartLevel, 10.0f);

	
}

void AKillingFloorGameModeBase::ShowPlayerHitWidget()
{
	//playerHit_UI = CreateWidget<UUserWidget>(GetWorld(), playerHitWidget);
	//if (playerHit_UI)
	//{
		// HMD 가 연결되어 있지 않다면
		//if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
		//{
		//	playerHit_UI->AddToViewport();

		//	FTimerHandle widgetTH;
		//	GetWorldTimerManager().SetTimer(widgetTH, this, &AKillingFloorGameModeBase::RemovePlayerHitWidget, 1.0f);
		//}
		//else
		//{
		//	if (playerHitWidget)
		//	{
		//		player->subWidgetComp->SetWidgetClass(playerHitWidget);
		//		player->subWidgetComp->InitWidget();
		//	}
		//	/*UWidgetComponent* hitWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("hitWidgetComp"));
		//	if (playerHitWidget)
		//	{
		//		hitWidgetComp->SetWidgetClass(playerHitWidget);
		//		hitWidgetComp->SetupAttachment(player->VRCamera);
		//		hitWidgetComp->SetRelativeLocationAndRotation(FVector(100, 0, 0), FRotator(0, 180, 0));
		//	}*/
		//	//mainWidgetComp->InitWidget();
		//	//layerHit_UI->SetPositionInViewport(FVector2D(0, 0));
		//	
		//}
		//player->subWidgetComp->SetWidgetClass(playerHitWidget);
		//player->subWidgetComp->InitWidget();
		
		
	//}

	//if (playerHitWidget)
	//{
		

		//FTimerHandle widgetTH;
		//GetWorldTimerManager().SetTimer(widgetTH, this, &AKillingFloorGameModeBase::RemovePlayerHitWidget, 1.0f);
		//player->subWidgetComp->InitWidget();
		//auto hitWidget = player->subWidgetComp->GetUserWidgetObject();
		//hitWidget->PlayAnimation();
	//}
	if (isShowHitUI)
	{
		return;
	}
	

	player->damageUI->SetVisibility(true);
	isShowHitUI = true;

	pcimatCollection = GetWorld()->GetParameterCollectionInstance(matCollection);
	if (pcimatCollection)
	{
		alpha = 0;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda(
			[this]()->void
			{
				//alpha = FMath::Lerp<float>(alpha, 1.0f, alphaSpeed * GetWorld()->DeltaTimeSeconds);
				
				alpha += alphaSpeed * GetWorld()->DeltaTimeSeconds;

				pcimatCollection->SetScalarParameterValue(FName("AlphaScalar"), FMath::Sin(alpha));

				if (FMath::Sin(alpha) < 0.0f)
				{
					pcimatCollection->SetScalarParameterValue(FName("AlphaScalar"), 0.0f);
					isShowHitUI = false;
					player->damageUI->SetVisibility(false);
					GetWorldTimerManager().ClearTimer(TimerHandle);
				}
			}
		), 0.02f, true);
	}
}

void AKillingFloorGameModeBase::RemovePlayerHitWidget()
{
	//player->subWidgetComp->SetWidgetClass(NULL);

	//playerHit_UI->RemoveFromParent();
}

void AKillingFloorGameModeBase::OpenStartLevel()
{
	GetWorldTimerManager().ClearTimer(widgetAlphaTimer);

	UGameplayStatics::OpenLevel(this, FName(TEXT("KF000")));
}

void AKillingFloorGameModeBase::FindShop()
{
	// 검색으로 찾은 결과를 저장할 배열
	TArray<AActor*> allActors;
	// 원하는 타입의 액터 모두 찾아오기
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), allActors);
	// 찾은 결과가 있을 경우 반복적으로
	for (auto spawn : allActors)
	{
		// 찾은 액터의 이름에 해당 문자열을 포함하고 있다면
		if (spawn->GetName().Contains(TEXT("Store")))
		{
			shop = spawn;
		}
	}
}

void AKillingFloorGameModeBase::PlayZedTime(float playRate)
{
	float drawNumber = FMath::RandRange(0.1f, 1.0f);
	if (drawNumber < playRate)
	{
		GetWorldSettings()->SetTimeDilation(0.2f);
		//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.5f);
		if (player)
		{
			player->CustomTimeDilation = 5.0f;
		}

		GetWorldTimerManager().ClearTimer(zedTimeHandle);
		GetWorldTimerManager().SetTimer(zedTimeHandle, this, &AKillingFloorGameModeBase::PlayOriginalTime, 1.0f, false, 3.0f / 5.0f);

		GetWorldTimerManager().ClearTimer(zedWidgetAlphaTimer);
		player->backgroundColor->SetVisibility(true);

		pcimatCollection = GetWorld()->GetParameterCollectionInstance(matCollection);
		if (pcimatCollection)
		{
			zedWidgetAlpha = 0;
			zedEasingDuration = 0;
			pcimatCollection->SetVectorParameterValue(FName("ColorVector"), FLinearColor::Gray);

			GetWorldTimerManager().SetTimer(zedWidgetAlphaTimer, FTimerDelegate::CreateLambda(
				[this]()->void
				{
					zedWidgetAlpha += zedWidgetAlphaSpeed * GetWorld()->DeltaTimeSeconds;
					zedEasingDuration += GetWorld()->DeltaTimeSeconds;
					
					pcimatCollection->SetScalarParameterValue(FName("BackgroundAlphaScalar"), FMath::Clamp(FMath::Sin(zedWidgetAlpha), 0.0f, 0.3f));

					if (zedEasingDuration > 3.0f / 5.0f)
					{
						pcimatCollection->SetScalarParameterValue(FName("BackgroundAlphaScalar"), 0.0f);
						player->backgroundColor->SetVisibility(false);
						GetWorldTimerManager().ClearTimer(zedWidgetAlphaTimer);
					}
				}
			), 0.02f, true);

		}
	}
}

void AKillingFloorGameModeBase::PlayOriginalTime()
{
	GetWorldSettings()->SetTimeDilation(1.0f);
	player->CustomTimeDilation = 1.0f;
}
