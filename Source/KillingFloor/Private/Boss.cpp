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

	// 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Enemy/Boss/T-Pose.T-Pose'"));
	// 1.1 데이터 로드 성공하면
	if (tempMesh.Succeeded())
	{
		//1.2 데이터 할당
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		//1.3 메시 위치 및 회전 설정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
		//1.4 메시 크기 수정
		GetMesh()->SetRelativeScale3D(FVector(0.84f));
	}
	// 애니메이션
	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/ABP_Boss.ABP_Boss_C'"));

	// 애니메이션 블루프린트 할당하기
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}
	 //BossFSM 컴포넌트 추가
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

