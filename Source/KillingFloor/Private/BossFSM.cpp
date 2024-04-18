// Fill out your copyright notice in the Description page of Project Settings.


#include "BossFSM.h"
#include "VRPlayer.h"
#include "Boss.h"
#include <Kismet/GameplayStatics.h>
#include "KillingFloorGameModeBase.h"
#include "BossAnim.h"
#include <AIController.h>
#include "PlayerMainWidget.h"
#include "VRPlayer.h"

UBossFSM::UBossFSM()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBossFSM::BeginPlay()
{
	Super::BeginPlay();

	// 월드에서 AVRPlayer 타깃 찾아오기
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), AVRPlayer::StaticClass());
	//AVRPlayer 타입으로 캐스팅
	target = Cast<AVRPlayer>(actor);
	//소유 객체 가져오기
	Me = Cast<ABoss>(GetOwner());
	// UEnemyAnim* 할당
	anim = Cast<UBossAnim>(Me->GetMesh()->GetAnimInstance());

	GameMode = Cast<AKillingFloorGameModeBase>(GetWorld()->GetAuthGameMode());
	// Ai 컨트롤
	ai = Cast<AAIController>(Me->GetController());
}

void UBossFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// 내 본체의 BossAnim State에 내 State를 넣어주고 싶다.

	Me->BossAnim->state = this->mstate;

	switch (mstate)
	{
	case EBossState::Idle:
		IdleState();
		break;
	case EBossState::Move:
		MoveState();
		break;
	case EBossState::Attack:
		AttackState();
		break;
	case EBossState::Damage:
		DamageState();
		break;
	case EBossState::Die:
		DieState();
		break;
	}
}

//대기상태
void UBossFSM::IdleState()
{
	// 1. 시간이 흘렀으니
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 만약 경과 시간이 대기 시간을 초과 했다면
	if (currentTime > idleDelayTime)
	{
		// 3. 이동 상태로 전환하고 싶다.
		mstate = EBossState::Move;
		// 경과 시간 초기화
		currentTime = 0;
		// 애니메이션 상태 동기화
		anim->state = mstate;
	}
}

// 이동상태
void UBossFSM::MoveState()
{
	// 1. 타깃 목적지가 필요하다.
	FVector destination = target->GetActorLocation();
	// 2. 방향이 필요하다.
	FVector dir = destination - Me->GetActorLocation();
	// 3. 방향으로 이동하고 싶다.
	//Me->AddMovementInput(dir.GetSafeNormal());

	// Ai nav mesh
	ai->MoveToLocation(destination);
	
	// 타깃과 가까워지면 공격 상태로 전환하고 싶다.
	// 1. 만약 거리가 공격범위 안에 들어오면
	if (dir.Size() < attackRange)
	{
		//2. 공격 상태로 전환하고 싶다.
		mstate = EBossState::Attack;
		// 애니메이션 상태 동기화
		anim->state = mstate;
		// 공격 애니메이션 재생 활성화
		anim->bAttackPlay =true;
		// 공격 상태 전환시 대기시간이 바로 끝나도록 처리
		currentTime = attackDelayTime;
	}
}

// 공격상태
void UBossFSM::AttackState()
{
	// 목표 : 일정 시간에 한 번씩 공격하고 싶다.
	// 1. 시간이 흘러야 한다.
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 공격 시간이 됐으니까
	if (currentTime > attackDelayTime)
	{
		// 3. 공격하고 싶다.
		//UE_LOG(LogTemp, Warning, TEXT("Attack"));
		GameMode->ShowPlayerHitWidget();
		target->HP -= 10;
		if (target->HP <= 0)
		{
			target->HP = 0;
		}
		GameMode->main_UI->PrintPlayerHP();

		if (target->HP <= 0)
		{
			GameMode->ShowDefeatWidget();
		}

		// 경과 시간 초기화
		currentTime = 0;
		anim->bAttackPlay = true;
	}
	// 목표 : 타깃이 공격 범위를 벗아나면 상태를 이동으로 전환하고 싶다.
	// 1. 타깃과의 거리가 필요하다.
	float distance = FVector::Distance(target->GetActorLocation(), Me->GetActorLocation());
	// 2. 타깃과의 거리가 공격 범위를 벗어났으니까
	if (distance > attackRange)
	{
		// 3. 상태를 이동으로 전환하고 싶다.
		mstate = EBossState::Move;
		// 애니메이션 상태 동기화
		anim->state = mstate;
	}
}
// 피격상태
void UBossFSM::DamageState()
{
	// 1. 시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 만약 경과 시간이 대기 시간을 초과했다면
	if (currentTime > damageDelayTime)
	{
		// 3. 대기 상태로 전황하고 싶다.
		mstate = EBossState::Idle;
		// 경과 시간 초기화
		currentTime = 0;
		// 애니메이션 상태 동기화
		anim->state = mstate;
	}
}
// 죽음상태
void UBossFSM::DieState()
{
	// 죽어가는 애니메이션이 끝나지 않았다면 계속 쓰러지고 싶다.
	if (anim->bDieDone == false)
	{
		return;
	}
	Me->Destroy();


}

// 맞는걸 알리는 이벤트 함수
void UBossFSM::OnDamageProcess(int32 damageValue)
{
	// 체력 감소
	hp -= damageValue;
	// 만약 체력이 남아있다면
	if (hp > 0)
	{
		// 상태를 피격 상황으로 변환
		mstate = EBossState::Damage;

		currentTime = 0;
		// 피격 애니메이션 재생
		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), 0);
		anim->PlayDamageAnim(FName(*sectionName));
	}
	// 그렇지 않다면
	else
	{
		GameMode->SubtractCurrentEnemyLeft();
		GameMode->CheckEnemyLeft();
		target->money += 1000;
		GameMode->main_UI->PrintPlayerMoney();
		
		//죽음 애니메이션 재생
		anim->PlayDamageAnim(TEXT("die"));
		// 상태를 죽음으로 전환
		mstate = EBossState::Die;

		isdying = true;

		// 승리 위젯
		GameMode->ShowVictoryWidget();
	}
}

