// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BossFSM.h"
#include "BossAnim.generated.h"

/**
 * 
 */
UCLASS()
class KILLINGFLOOR_API UBossAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	EBossState state;
	//공격상태 재생할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	bool bAttackPlay = false;

	// 공격 애니메이션 끝나는 이벤트 함수
	UFUNCTION(BlueprintCallable, Category = FSMEvent)
		void OnEndAttackAnimation();
	
	// 피격 애니메이션 재생함수
	UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)
		void PlayDamageAnim(FName sectionName);

	// 죽음상태 애니메이션 종료여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		bool bDieDone = false;

};
