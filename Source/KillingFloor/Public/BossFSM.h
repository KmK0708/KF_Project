// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossFSM.generated.h"

// 사용할 상태 정의
UENUM(BlueprintType)
enum class EBossState : uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KILLINGFLOOR_API UBossFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBossFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	// 상태변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
		EBossState mstate = EBossState::Idle;

	//대기상태
	void IdleState();
	//이동상태
	void MoveState();
	//공격상태
	void AttackState();
	//피격상태
	void DamageState();
	//죽음상태
	void DieState();

	// 대기시간
	UPROPERTY(EditDefaultsOnly, Category = FSM)
		float idleDelayTime = 1;
	// 경과시간
	float currentTime = 0;

	// 타깃쪽으로 향한다.
	UPROPERTY(VisibleAnywhere, Category = FSM)
		class AVRPlayer* target;

	// 소유 엑터
	UPROPERTY()
		class ABoss* Me;

	// 공격 범위
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackRange = 150.0f;

	// 공격 대기 시간
	UPROPERTY(EditAnywhere, Category = FSM)
		float attackDelayTime = 3.0f;

	// 맞는 걸 알리는 이벤트 함수
	void OnDamageProcess(int32 damageValue);

	//체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		int32 hp = 40;

	//피격 대기 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		float damageDelayTime = 1.0f;

	//쓰러지는 속도
	UPROPERTY(EditAnywhere, Category = FSM)
		float dieSpeed = 50.0f;
	// 사용중인 애니메이션 블루프린트
	UPROPERTY()
	class UBossAnim* anim;

	bool isdying = false;

	UPROPERTY()
	class AKillingFloorGameModeBase* GameMode;

	// Ai 컨트롤
	UPROPERTY()
	class AAIController* ai;
};
		

