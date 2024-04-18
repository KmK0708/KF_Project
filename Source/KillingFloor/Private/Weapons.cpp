// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons.h"

// Sets default values
AWeapons::AWeapons()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}



// Called when the game starts or when spawned
void AWeapons::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapons::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void AWeapons::FireWeaponPistol()
//{
	//// 무기 발사 로직
	//// /Script/Engine.StaticMesh'/Game/3DObject/Pistol/M1911.M1911'
	//FHitResult hitInfo;
	//FVector StartTrace = VRCamera->GetComponentLocation();
	//FVector EndTrace = StartTrace + VRCamera->GetForwardVector() * 10000.0f;
	//FCollisionObjectQueryParams objParams;
	//FCollisionObjectQueryParams objParams1;

	//bool bHit = GetWorld()->LineTraceSingleByObjectType(hitInfo, StartTrace, EndTrace,objParams);

	//// 맞았으면
	//if (bHit)
	//{
	//	FTransform Trans(hitInfo.ImpactPoint);
	//	FVector Loc(hitInfo.ImpactPoint);

	//	SpawnEmitter(pistolEnemyImpactFactory, trans);
	//	PlaySound(enemyImpactSound, loc);

	//	auto enemy = Cast<AEnemy>(hitInfo.GetActor());
	//	if (enemy != nullptr)
	//	{
	//		
	//	}
	//}

//}

//void AWeapons::FireWeaponRifle()
//{
	//// 무기 발사 로직
	//// /Script/Engine.StaticMesh'/Game/3DObject/Pistol/M1911.M1911'
	//FHitResult hitInfo;
	//FVector StartTrace = VRCamera->GetComponentLocation();
	//FVector EndTrace = StartTrace + VRCamera->GetForwardVector() * 10000.0f;
	//FCollisionObjectQueryParams objParams;
	//FCollisionObjectQueryParams objParams1;
	//bool bHit = GetWorld()->LineTraceSingleByObjectType(hitInfo, StartTrace, EndTrace,objParams);

	//// 맞았으면
	//if (bHit)
	//{
	//	
	//}
//}

//void AWeapons::FireWeaponLauncher()
//{

//}
