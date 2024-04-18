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
	// 라이플 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	class UNiagaraComponent* MuzzleFireNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
		class UStaticMeshComponent* RifleMesh;

	// 픽업 반경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
		class USphereComponent* PickupSphere;

	// 권총이 맞았을 때 생성할 파티클
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
		class UParticleSystem* HitParticle;

	// 적이 총에 맞았을 때 생성할 파티클
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
		class UParticleSystem* HitEnemyParticle;

	// 총구 화염 niagara
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	class UNiagaraSystem* MuzzleFire;

	// Gamemodebase
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GM")
		class AKillingFloorGameModeBase* GameMode;

	// 총 연사기능
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	float FireRate = 0.1f;
	// 발사시간
	FTimerHandle FireTimer;

	// 총 발사 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rifle")
	class USoundBase* RifleFireSound;

		// 총 재장전 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class USoundBase* RifleReloadSound;

	// Handgun 탄약
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		int32 RifleAmmo;

	// Handgun 탄창 최대치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		int32 RifleAmmoMax = 30;


		// CameraShake
	UPROPERTY(EditAnywhere, Category = "VRCamera")
	TSubclassOf<class UCameraShakeBase> RiflecameraShakeFactory;
	UPROPERTY()
	class UCameraShakeBase* RiflecanShakeInstance;

	// 헤더 파일에서 피 데칼 액터의 클래스를 선언합니다.
	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* DecalMaterial;

		// 권총 탄약 다시 채우기 함수
	UFUNCTION(BlueprintCallable)
	void ReloadRifle();

	// 장전시간 함수
	UFUNCTION()
	void ReloadTime();

	// 총 발사 가능여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		bool bCanFire = false;

	// 오버랩비긴
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 권총 발사 함수
	UFUNCTION(BlueprintCallable)
		void FireRifle();

	bool bIsPickedUp = false;
};
