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
	// 권총 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class UStaticMeshComponent* HandgunMesh;

	// 픽업 반경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class USphereComponent* PickupSphere;

	// 권총이 맞았을 때 생성할 파티클
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class UParticleSystem* HitParticle;

	// 권총에 적이 맞았을 때 생성할 파티클
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class UParticleSystem* HitEnemyParticle;


	// 총구 화염
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class UParticleSystem* MuzzleFlash;

	// Gamemodebase
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GM")
		class AKillingFloorGameModeBase* GameMode;

	// 총 발사 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class USoundBase* HandgunFireSound;

	// 총 재장전 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		class USoundBase* HandgunReloadSound;

	// Handgun 탄약
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		int32 HandgunAmmo;

	// Handgun 탄창 최대치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		int32 HandgunAmmoMax = 7;

	// 오버랩비긴
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 권총 발사 함수
	UFUNCTION()
	void FirePistol();

	// 권총 탄약 다시 채우기 함수
	UFUNCTION(BlueprintCallable)
	void ReloadPistol();

	// 장전시간 함수
	UFUNCTION()
	void ReloadTime();

	// 총 발사 가능여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handgun")
		bool bCanFire = false;

	bool bIsPickedUp = false;

		// 헤더 파일에서 피 데칼 액터의 클래스를 선언합니다.
	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* DecalMaterial;
	// 적을 죽이면 돈이 증가하는 함수
	//UFUNCTION()
	//void IncreaseMoney();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FString Name;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float Price;
};
