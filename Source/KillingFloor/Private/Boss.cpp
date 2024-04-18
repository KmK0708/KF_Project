// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"

#include "BossAnim.h"
#include "BossFSM.h"
#include "EnemyAnim.h"

// Sets default values
ABoss::ABoss()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ���̷�Ż �޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Enemy/Boss/T-Pose.T-Pose'"));
	// 1.1 ������ �ε� �����ϸ�
	if (tempMesh.Succeeded())
	{
		//1.2 ������ �Ҵ�
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		//1.3 �޽� ��ġ �� ȸ�� ����
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		//1.4 �޽� ũ�� ����
		GetMesh()->SetRelativeScale3D(FVector(0.84f));
	}
	// �ִϸ��̼�
	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/ABP_Boss.ABP_Boss_C'"));

	// �ִϸ��̼� �������Ʈ �Ҵ��ϱ�
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}
	 //BossFSM ������Ʈ �߰�
	FSM = CreateDefaultSubobject<UBossFSM>(TEXT("FSM"));


}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();
	BossAnim = Cast<UBossAnim>(GetMesh()->GetAnimInstance());
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

