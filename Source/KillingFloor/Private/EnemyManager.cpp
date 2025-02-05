// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
#include "EnemyZombie.h"
#include <EngineUtils.h>
// Sets default values
AEnemyManager::AEnemyManager()
{
	// Tick() 함수가 매 프레임 호출되지 않도록 설정
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	// 1. 랜덤 생성 시간 구하기
	float createTime = FMath::RandRange(minTime, maxTime);
	// 2. Timer Manager 한테 알람 등록
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, createTime);
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyManager::CreateEnemy()
{
	//랜덤 위치 구하기
	int index = FMath::RandRange(0, spawnPoints.Num()-1);
	//적 생성 및 배치하기
	GetWorld()->SpawnActor<AEnemyZombie>(enemyFactory, spawnPoints[index]->GetActorLocation(),FRotator(0));
	// 다시 랜덤 시간에 CreateEnemy 함수가 호출되도록 타이머 설정
	float createTime = FMath::RandRange(minTime, maxTime);
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, createTime);
}

void AEnemyManager::FindSpawnPoints()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* spawn = *It;
		// 찾은 액터의 이름에 해당 문자열을 포함하고 있다면
		if (spawn->GetName().Contains(TEXT("BP_EnemeySpawnPoint")))
		{
			// 스폰 목록에 추가
			spawnPoints.Add(spawn);
		}
	}
}

