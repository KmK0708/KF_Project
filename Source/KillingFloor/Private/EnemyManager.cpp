// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
#include "EnemyZombie.h"
#include <EngineUtils.h>
// Sets default values
AEnemyManager::AEnemyManager()
{
	// Tick() �Լ��� �� ������ ȣ����� �ʵ��� ����
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	// 1. ���� ���� �ð� ���ϱ�
	float createTime = FMath::RandRange(minTime, maxTime);
	// 2. Timer Manager ���� �˶� ���
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, createTime);
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyManager::CreateEnemy()
{
	//���� ��ġ ���ϱ�
	int index = FMath::RandRange(0, spawnPoints.Num()-1);
	//�� ���� �� ��ġ�ϱ�
	GetWorld()->SpawnActor<AEnemyZombie>(enemyFactory, spawnPoints[index]->GetActorLocation(),FRotator(0));
	// �ٽ� ���� �ð��� CreateEnemy �Լ��� ȣ��ǵ��� Ÿ�̸� ����
	float createTime = FMath::RandRange(minTime, maxTime);
	GetWorld()->GetTimerManager().SetTimer(spawnTimerHandle, this, &AEnemyManager::CreateEnemy, createTime);
}

void AEnemyManager::FindSpawnPoints()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* spawn = *It;
		// ã�� ������ �̸��� �ش� ���ڿ��� �����ϰ� �ִٸ�
		if (spawn->GetName().Contains(TEXT("BP_EnemeySpawnPoint")))
		{
			// ���� ��Ͽ� �߰�
			spawnPoints.Add(spawn);
		}
	}
}

