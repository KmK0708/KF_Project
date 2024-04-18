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
	// 런처 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Launcher")
		class UStaticMeshComponent* LauncherMesh;

	// 픽업 반경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Launcher")
		class USphereComponent* PickupSphere;
	// 총구 화염
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Launcher")
		class UParticleSystem* MuzzleFlash;

	// 런처를 쏠때 RocketProjectile 스폰
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Launcher")
		TSubclassOf<class ARocketProjectile> RocketProjectileClass;

	// 런처 발사 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Launcher")
		class USoundBase* LauncherFireSound;


	// 오버랩비긴
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 런처 발사 함수
	UFUNCTION()
		void FireLauncher();



	bool bIsPickedUp = false;
};
