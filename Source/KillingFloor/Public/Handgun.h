// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Handgun.generated.h"

UCLASS()
class KILLINGFLOOR_API AHandgun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandgun();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	// ���� �޽�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class UStaticMeshComponent* HandgunMesh;

	// �Ⱦ� �ݰ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class USphereComponent* PickupSphere;

	// ������ �¾��� �� ������ ��ƼŬ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class UParticleSystem* HitParticle;

	// ���ѿ� ���� �¾��� �� ������ ��ƼŬ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class UParticleSystem* HitEnemyParticle;


	// �ѱ� ȭ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class UParticleSystem* MuzzleFlash;

	// Gamemodebase
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GM")
		class AKillingFloorGameModeBase* GameMode;

	// �� �߻� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class USoundBase* HandgunFireSound;

	// �� ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class USoundBase* HandgunReloadSound;

	// Handgun ź��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		int32 HandgunAmmo;

	// Handgun źâ �ִ�ġ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		int32 HandgunAmmoMax = 7;

	// ���������
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ���� �߻� �Լ�
	UFUNCTION()
	void FirePistol();

	// ���� ź�� �ٽ� ä��� �Լ�
	UFUNCTION(BlueprintCallable)
	void ReloadPistol();

	// �����ð� �Լ�
	UFUNCTION()
	void ReloadTime();

	// �� �߻� ���ɿ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		bool bCanFire = false;

	bool bIsPickedUp = false;

		// ��� ���Ͽ��� �� ��Į ������ Ŭ������ �����մϴ�.
	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* DecalMaterial;
	// ���� ���̸� ���� �����ϴ� �Լ�
	//UFUNCTION()
	//void IncreaseMoney();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FString Name;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float Price;
};
