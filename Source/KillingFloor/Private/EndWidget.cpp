// Fill out your copyright notice in the Description page of Project Settings.


#include "EndWidget.h"
#include "Components/Border.h"
#include "Engine/World.h"

void UEndWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
	//GetWorld()->GetTimerManager().SetTimer(alphaTimerHandle, FTimerDelegate::CreateLambda(
	//	[this]()->void
	//	{
	//		curAlpha += alphaSpeed * GetWorld()->DeltaTimeSeconds;
	//		//curAlpha = FMath::Lerp<float>(curAlpha, 1.0f, 1.0f * GetWorld()->DeltaTimeSeconds);
	//			
	//		//float easingAlpha = EaseOutBounce(curAlpha);
	//		//float easingAlpha = 0;
	//		//easingAlpha = FMath::Lerp<float>(easingAlpha, 0.8f, EaseOutBounce(easingAlpha) * GetWorld()->DeltaTimeSeconds);
	//		UE_LOG(LogTemp, Warning, TEXT("%f %f"), FMath::Sin(curAlpha), curAlpha);
	//		background->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, FMath::Sin(curAlpha)));

	//		if (FMath::Sin(curAlpha) < 0.0f)
	//		{
	//			background->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
	//			GetWorld()->GetTimerManager().ClearTimer(alphaTimerHandle);
	//		}
	//	}
	//), 0.02f, true);
	
}

float UEndWidget::EaseOutBounce(float t)
{
	//const float n1 = 7.5625f;
	//const float d1 = 2.75f;

	//if (t < 1 / d1)
	//{
	//	return n1 * t * t;
	//}
	//else if (t < 2 / d1)
	//{
	//	t -= 1.5 / d1;
	//	return n1 * t * t + 0.75;
	//}
	//else if (t < 2.5 / d1)
	//{
	//	t -= 2.25 / d1;
	//	return n1 * t * t + 0.9375;
	//}
	//else
	//{
	//	t -= 2.625 / d1;
	//	return n1 * t * t + 0.984375;
	//}

	//return 1 - FMath::Pow(2, -6 * t) * FMath::Abs(FMath::Cos(t * PI * 3.5));

	t *= t;
	return t * t;
}

