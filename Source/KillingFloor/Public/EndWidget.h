// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndWidget.generated.h"

/**
 * 
 */
UCLASS()
class KILLINGFLOOR_API UEndWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color", meta = (BindWidget))
	class UBorder* background;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float alphaSpeed = 5;

private:
	float curAlpha = 0;
	float prevEasingAlpha = 0;
	float nextAlpha = -0.3f;

	FTimerHandle alphaTimerHandle;

	float EaseOutBounce(float t);
	
};

