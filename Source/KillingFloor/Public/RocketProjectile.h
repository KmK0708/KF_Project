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
	// 발사체 메시
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RocketMesh")
		class UStaticMeshComponent* RocketMesh;

	// 발사체 파티클
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
		class UParticleSystem* RocketParticle;

	// 폭발 소리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
		class USoundBase* RocketBoomSound;

	// 구체 콜리전
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RocketCollision")
		class USphereComponent* RocketCollision;

	// 발사체 무브먼트 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RocketMovement")
		class UProjectileMovementComponent* RocketMovement;

	// 반경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
		float ExplosionRadius = 500.0f;
	
	// 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
		float ExplosionDamage = 10.0f;

	// 게임모드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GM")
		class AKillingFloorGameModeBase* GameMode;

	// 터질때 카메라 흔들기
	UPROPERTY(EditAnywhere, Category = "VRCamera")
	TSubclassOf<class UCameraShakeBase> ExcameraShakeFactory;

	UPROPERTY()
	class UCameraShakeBase* ExcanShakeInstance;

			// 헤더 파일에서 피 데칼 액터의 클래스를 선언합니다.
	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* DecalMaterial;


	// 로켓콜리전이 오버랩되었을때 나오는 함수
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
