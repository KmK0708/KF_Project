// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rifle.generated.h"

UCLASS()
class KILLINGFLOOR_API ARifle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARifle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// ������ �޽�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	class UNiagaraComponent* MuzzleFireNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
		class UStaticMeshComponent* RifleMesh;

	// �Ⱦ� �ݰ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
		class USphereComponent* PickupSphere;

	// ������ �¾��� �� ������ ��ƼŬ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
		class UParticleSystem* HitParticle;

	// ���� �ѿ� �¾��� �� ������ ��ƼŬ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
		class UParticleSystem* HitEnemyParticle;

	// �ѱ� ȭ�� niagara
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	class UNiagaraSystem* MuzzleFire;

	// Gamemodebase
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GM")
		class AKillingFloorGameModeBase* GameMode;

	// �� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	float FireRate = 0.1f;
	// �߻�ð�
	FTimerHandle FireTimer;

	// �� �߻� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	class USoundBase* RifleFireSound;

		// �� ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class USoundBase* RifleReloadSound;

	// Handgun ź��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		int32 RifleAmmo;

	// Handgun źâ �ִ�ġ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		int32 RifleAmmoMax = 30;


		// CameraShake
	UPROPERTY(EditAnywhere, Category = "VRCamera")
	TSubclassOf<class UCameraShakeBase> RiflecameraShakeFactory;
	UPROPERTY()
	class UCameraShakeBase* RiflecanShakeInstance;

	// ��� ���Ͽ��� �� ��Į ������ Ŭ������ �����մϴ�.
	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* DecalMaterial;

		// ���� ź�� �ٽ� ä��� �Լ�
	UFUNCTION(BlueprintCallable)
	void ReloadRifle();

	// �����ð� �Լ�
	UFUNCTION()
	void ReloadTime();

	// �� �߻� ���ɿ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		bool bCanFire = false;

	// ���������
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ���� �߻� �Լ�
	UFUNCTION(BlueprintCallable)
		void FireRifle();

	bool bIsPickedUp = false;
};
