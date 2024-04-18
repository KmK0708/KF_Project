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

	// ���忡�� AVRPlayer Ÿ�� ã�ƿ���
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), AVRPlayer::StaticClass());
	//AVRPlayer Ÿ������ ĳ����
	target = Cast<AVRPlayer>(actor);
	//���� ��ü ��������
	Me = Cast<ABoss>(GetOwner());
	// UEnemyAnim* �Ҵ�
	anim = Cast<UBossAnim>(Me->GetMesh()->GetAnimInstance());

	GameMode = Cast<AKillingFloorGameModeBase>(GetWorld()->GetAuthGameMode());
	// Ai ��Ʈ��
	ai = Cast<AAIController>(Me->GetController());
}

void UBossFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// �� ��ü�� BossAnim State�� �� State�� �־��ְ� �ʹ�.

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

//������
void UBossFSM::IdleState()
{
	// 1. �ð��� �귶����
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. ���� ��� �ð��� ��� �ð��� �ʰ� �ߴٸ�
	if (currentTime > idleDelayTime)
	{
		// 3. �̵� ���·� ��ȯ�ϰ� �ʹ�.
		mstate = EBossState::Move;
		// ��� �ð� �ʱ�ȭ
		currentTime = 0;
		// �ִϸ��̼� ���� ����ȭ
		anim->state = mstate;
	}
}

// �̵�����
void UBossFSM::MoveState()
{
	// 1. Ÿ�� �������� �ʿ��ϴ�.
	FVector destination = target->GetActorLocation();
	// 2. ������ �ʿ��ϴ�.
	FVector dir = destination - Me->GetActorLocation();
	// 3. �������� �̵��ϰ� �ʹ�.
	//Me->AddMovementInput(dir.GetSafeNormal());

	// Ai nav mesh
	ai->MoveToLocation(destination);
	
	// Ÿ��� ��������� ���� ���·� ��ȯ�ϰ� �ʹ�.
	// 1. ���� �Ÿ��� ���ݹ��� �ȿ� ������
	if (dir.Size() < attackRange)
	{
		//2. ���� ���·� ��ȯ�ϰ� �ʹ�.
		mstate = EBossState::Attack;
		// �ִϸ��̼� ���� ����ȭ
		anim->state = mstate;
		// ���� �ִϸ��̼� ��� Ȱ��ȭ
		anim->bAttackPlay =true;
		// ���� ���� ��ȯ�� ���ð��� �ٷ� �������� ó��
		currentTime = attackDelayTime;
	}
}

// ���ݻ���
void UBossFSM::AttackState()
{
	// ��ǥ : ���� �ð��� �� ���� �����ϰ� �ʹ�.
	// 1. �ð��� �귯�� �Ѵ�.
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. ���� �ð��� �����ϱ�
	if (currentTime > attackDelayTime)
	{
		// 3. �����ϰ� �ʹ�.
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

		// ��� �ð� �ʱ�ȭ
		currentTime = 0;
		anim->bAttackPlay = true;
	}
	// ��ǥ : Ÿ���� ���� ������ ���Ƴ��� ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
	// 1. Ÿ����� �Ÿ��� �ʿ��ϴ�.
	float distance = FVector::Distance(target->GetActorLocation(), Me->GetActorLocation());
	// 2. Ÿ����� �Ÿ��� ���� ������ ������ϱ�
	if (distance > attackRange)
	{
		// 3. ���¸� �̵����� ��ȯ�ϰ� �ʹ�.
		mstate = EBossState::Move;
		// �ִϸ��̼� ���� ����ȭ
		anim->state = mstate;
	}
}
// �ǰݻ���
void UBossFSM::DamageState()
{
	// 1. �ð��� �귶���ϱ�
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. ���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	if (currentTime > damageDelayTime)
	{
		// 3. ��� ���·� ��Ȳ�ϰ� �ʹ�.
		mstate = EBossState::Idle;
		// ��� �ð� �ʱ�ȭ
		currentTime = 0;
		// �ִϸ��̼� ���� ����ȭ
		anim->state = mstate;
	}
}
// ��������
void UBossFSM::DieState()
{
	// �׾�� �ִϸ��̼��� ������ �ʾҴٸ� ��� �������� �ʹ�.
	if (anim->bDieDone == false)
	{
		return;
	}
	Me->Destroy();


}

// �´°� �˸��� �̺�Ʈ �Լ�
void UBossFSM::OnDamageProcess(int32 damageValue)
{
	// ü�� ����
	hp -= damageValue;
	// ���� ü���� �����ִٸ�
	if (hp > 0)
	{
		// ���¸� �ǰ� ��Ȳ���� ��ȯ
		mstate = EBossState::Damage;

		currentTime = 0;
		// �ǰ� �ִϸ��̼� ���
		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damage%d"), 0);
		anim->PlayDamageAnim(FName(*sectionName));
	}
	// �׷��� �ʴٸ�
	else
	{
		GameMode->SubtractCurrentEnemyLeft();
		GameMode->CheckEnemyLeft();
		target->money += 1000;
		GameMode->main_UI->PrintPlayerMoney();
		
		//���� �ִϸ��̼� ���
		anim->PlayDamageAnim(TEXT("die"));
		// ���¸� �������� ��ȯ
		mstate = EBossState::Die;

		isdying = true;

		// �¸� ����
		GameMode->ShowVictoryWidget();
	}
}

