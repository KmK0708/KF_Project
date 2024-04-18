// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMainWidget.h"
#include "KillingFloorGameModeBase.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "VRPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/Image.h"
#include "Components/Slider.h"

void UPlayerMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	gm = Cast<AKillingFloorGameModeBase>(GetWorld()->GetAuthGameMode());

	player = Cast<AVRPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (gm->main_UI != nullptr)
	{
		//gm->main_UI->PrintCurrentEnemyLeft();
		gm->main_UI->PrintRemainingTime();
		gm->main_UI->PrintCurrentWave();
		gm->main_UI->PrintLastWave();
		gm->main_UI->PrintPlayerHP();
		gm->main_UI->PrintPlayerMoney();
		gm->main_UI->PrintShopDirection();
		gm->main_UI->PrintGunCount();
		gm->main_UI->PrintShopDistance();
		gm->main_UI->PrintWaveImage(Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Script/Engine.Texture2D'/Game/Assets/UI/wall-clock.wall-clock'"))));
	}
}

void UPlayerMainWidget::PrintCurrentEnemyLeft()
{
	if (gm != nullptr)
	{
		if (gm->GetCurrentWave() == 3)
		{
			FString enemyBoss = TEXT("보스");
			currentEnemyLeft->SetText(FText::FromString(enemyBoss));
		}
		else
		{
			FText enemyLeft = FText::FromString(FString::FromInt(gm->GetCurrentEnemyLeft()));
			currentEnemyLeft->SetText(enemyLeft);
		}
	}
}

void UPlayerMainWidget::PrintRemainingTime()
{
	if (gm != nullptr)
	{
		FNumberFormattingOptions opts;
		opts.MinimumIntegralDigits = 2;
		opts.MaximumIntegralDigits = 2;
		FText remainingSeconds = FText::AsNumber(gm->Seconds, &opts);
		FString remainingTime = FString::Printf(TEXT("0:%s"), *remainingSeconds.ToString());
		currentEnemyLeft->SetText(FText::FromString(remainingTime));
	}
}

void UPlayerMainWidget::PrintCurrentWave()
{
	if (gm != nullptr)
	{
		FText curWave = FText::AsNumber(gm->GetCurrentWave());
		currentWave->SetText(curWave);
	}
}

void UPlayerMainWidget::PrintLastWave()
{
	if (gm != nullptr)
	{
		FText lWave = FText::AsNumber(gm->GetLastWave());
		lastWave->SetText(lWave);
	}
}

void UPlayerMainWidget::PrintPlayerHP()
{
	if (player != nullptr && gm != nullptr)
	{
		FText hp = FText::AsNumber(player->HP);
		playerHP->SetText(hp);
	}
}

void UPlayerMainWidget::PrintPlayerMoney()
{
	if (player != nullptr && gm != nullptr)
	{
		FText money = FText::AsNumber(player->money);
		playerMoney->SetText(money);
	}
}

void UPlayerMainWidget::PrintPlayerMoneyAnimation(int32 prevMoney, int nextMoney)
{
	if (isPrintMoney)
	{
		GetWorld()->GetTimerManager().ClearTimer(moneyTimerHandle);
		FText money = FText::AsNumber(player->money);
		playerMoney->SetText(money);
	}

	if (player != nullptr && gm != nullptr)
	{
		curMoney = prevMoney;

		isPrintMoney = true;
		GetWorld()->GetTimerManager().SetTimer(moneyTimerHandle, FTimerDelegate::CreateLambda(
			[this, nextMoney]()->void
			{
				curMoney += 13;

				FText money = FText::AsNumber(curMoney);
				playerMoney->SetText(money);

				if (curMoney >= player->money)
				{
					curMoney = player->money;
					playerMoney->SetText(FText::AsNumber(curMoney));
					isPrintMoney = false;
					GetWorld()->GetTimerManager().ClearTimer(moneyTimerHandle);
				}
			}
		), 0.02, true);
	}
}

void UPlayerMainWidget::PrintWaveImage(UTexture2D* waveTexture)
{
	waveImage->SetBrushFromTexture(waveTexture);
}

void UPlayerMainWidget::PrintShopDirection()
{
	if (player != nullptr && gm != nullptr)
	{
		//FVector v = player->GetActorForwardVector();
		FVector v = player->VRCamera->GetForwardVector();
		FVector w = (gm->shop->GetActorLocation() - player->GetActorLocation()).GetSafeNormal();

		float cosTheta = FVector::DotProduct(v, w);
		float dir = FVector::DotProduct(player->GetActorUpVector(), FVector::CrossProduct(v, w));

		float value = 0.0f;
		// 정면
		if (dir == 0)
		{
			value = 0.5f;
		}
		// 왼쪽
		else if (dir < 0)
		{
			value = (cosTheta + 1) / 4;
		}
		// 오른쪽
		else
		{
			value = (1 - cosTheta) / 4 + 0.5f;
		}
		shopDirection->SetValue(value);
	}

}

void UPlayerMainWidget::PrintGunCount()
{
	if (player)
	{
		FText gunCnt = FText::AsNumber(player->gunCount);
		gunCount->SetText(gunCnt);
	}
}

void UPlayerMainWidget::PrintEarnMoney(int32 money)
{
	if (player)
	{
		earnMoney->SetText(FText::AsNumber(money));
		PlayAnimation(EarnMoneyAnimation, 0, 1);
	}
}

void UPlayerMainWidget::PrintShopDistance()
{
	if (player && gm)
	{
		UNavigationPath* navPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(), player->GetActorLocation(), gm->shop);
		float len = navPath->GetPathLength() * 0.01f;
		shopDistance->SetText(FText::AsNumber(FMath::FloorToInt32(len)));
	}
}


