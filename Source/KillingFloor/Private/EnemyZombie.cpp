// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyZombie.h"
#include "EnemyAnim.h"
#include "EnemyFSM.h"

// Sets default values
AEnemyZombie::AEnemyZombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 스켈레탈 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Enemy/Zombie/Yaku_J_Ignite.Yaku_J_Ignite'"));
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("can't find mesh"));
	}

	// 애니메이션
	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Enemy/Bluprint/ABP_EnemyZombie.ABP_EnemyZombie_C'"));

	// 애니메이션 블루프린트 할당하기
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}
	// EnemyFSM 컴포넌트 추가
	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	//스폰- 월드에 배치되거 스폰될때 자동으로 AiController부터 Possess될 수 있도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


}

// Called when the game starts or when spawned
void AEnemyZombie::BeginPlay()
{
	Super::BeginPlay();
	enemyAnim = Cast<UEnemyAnim>(GetMesh()->GetAnimInstance());
}

// Called every frame
void AEnemyZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyZombie::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

