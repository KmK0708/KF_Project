// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Launcher.generated.h"

UCLASS()
class KILLINGFLOOR_API ALauncher : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// ��ó �޽�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Launcher")
		class UStaticMeshComponent* LauncherMesh;

	// �Ⱦ� �ݰ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Launcher")
		class USphereComponent* PickupSphere;
	// �ѱ� ȭ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Launcher")
		class UParticleSystem* MuzzleFlash;

	// ��ó�� �� RocketProjectile ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Launcher")
		TSubclassOf<class ARocketProjectile> RocketProjectileClass;

	// ��ó �߻� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Launcher")
		class USoundBase* LauncherFireSound;


	// ���������
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ��ó �߻� �Լ�
	UFUNCTION()
		void FireLauncher();



	bool bIsPickedUp = false;
};
