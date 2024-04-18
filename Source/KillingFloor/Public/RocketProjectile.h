// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RocketProjectile.generated.h"

UCLASS()
class KILLINGFLOOR_API ARocketProjectile : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ARocketProjectile();

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UFUNCTION()
	//	void OnDetonate();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// �߻�ü �޽�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RocketMesh")
		class UStaticMeshComponent* RocketMesh;

	// �߻�ü ��ƼŬ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
		class UParticleSystem* RocketParticle;

	// ���� �Ҹ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
		class USoundBase* RocketBoomSound;

	// ��ü �ݸ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RocketCollision")
		class USphereComponent* RocketCollision;

	// �߻�ü �����Ʈ ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RocketMovement")
		class UProjectileMovementComponent* RocketMovement;

	// �ݰ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
		float ExplosionRadius = 500.0f;
	
	// ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
		float ExplosionDamage = 10.0f;

	// ���Ӹ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GM")
		class AKillingFloorGameModeBase* GameMode;

	// ������ ī�޶� ����
	UPROPERTY(EditAnywhere, Category = "VRCamera")
	TSubclassOf<class UCameraShakeBase> ExcameraShakeFactory;

	UPROPERTY()
	class UCameraShakeBase* ExcanShakeInstance;

			// ��� ���Ͽ��� �� ��Į ������ Ŭ������ �����մϴ�.
	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* DecalMaterial;


	// �����ݸ����� �������Ǿ����� ������ �Լ�
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
