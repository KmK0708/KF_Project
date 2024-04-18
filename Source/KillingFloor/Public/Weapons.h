// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons.generated.h"

UCLASS()
class KILLINGFLOOR_API AWeapons : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapons();

	// 모든 무기의 공통 속성 및 기능을 정의하는 클래스
	// 무기의 종류에 따라서 상속받아서 사용한다


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, Category = "Weapon")
    FName Name;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    float Cost;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    TSubclassOf<USkeletalMeshComponent> Mesh;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    TSubclassOf<UAnimInstance> AnimInstance;

	
};
