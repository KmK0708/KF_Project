
// Fill out your copyright notice in the Description page of Project Settings.


#include "Handgun.h"
#include <Kismet/GameplayStatics.h>
#include <Components/SphereComponent.h>
#include "DrawDebugHelpers.h"
#include "VRPlayer.h"
#include "EnemyZombie.h"
#include "EnemyFSM.h"
#include "Boss.h"
#include "BossFSM.h"
#include "MotionControllerComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/DecalActor.h"
#include "KillingFloorGameModeBase.h"
#include "PlayerMainWidget.h"

// Sets default values
AHandgun::AHandgun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HandgunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandgunMesh"));
	SetRootComponent(HandgunMesh);
	

	// 구체 콜리전 생성 및 설정 (픽업을 위한)
	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetSphereRadius(50.0f);
	PickupSphere->SetupAttachment(HandgunMesh);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	PickupSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// 권총 메쉬 설정
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempPistol(TEXT("/Script/Engine.StaticMesh'/Game/3DObject/Pistol/M1911.M1911'"));
	// 불러오는데 성공했다면..
	if (TempPistol.Succeeded())
	{
		// 권총 메쉬에 적용
		HandgunMesh->SetStaticMesh(TempPistol.Object);
	}

	bIsPickedUp = false;
}


// Called when the game starts or when spawned
void AHandgun::BeginPlay()
{
	Super::BeginPlay();
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AHandgun::OnOverlapBegin);
	GameMode = Cast<AKillingFloorGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	// 처음 탄약 이 최대 탄약과 같다.
	HandgunAmmo = HandgunAmmoMax;
}

// Called every frame
void AHandgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHandgun::FirePistol()
{
	// 만약 탄약이 0보다 크다면
	if (HandgunAmmo > 0)
	{
		bCanFire = true;
		UE_LOG(LogTemp, Warning, TEXT("FirePistol"));
		// 권총 발사
		FVector StartLocation = HandgunMesh->GetSocketLocation("PewPewPoint");
		FVector ForwardVector = HandgunMesh->GetForwardVector();		// 돌린이유:총이 오른쪽으로 꺾여서 나감
		FVector EndLocation = StartLocation + (ForwardVector.RotateAngleAxis(-90.f, HandgunMesh->GetUpVector()) * 10000.0f);
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
		// 총 쏠때 총구 앞에서 화염발생
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, HandgunMesh->GetSocketLocation("PewPewPoint"));

		// 총 발사 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HandgunFireSound, HandgunMesh->GetSocketLocation("PewPewPoint"));

		// 탄약을 1개 소모
		HandgunAmmo--;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params))
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				// 권총이 맞은 위치에 파티클 생성
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitResult.ImpactPoint, FRotator::ZeroRotator);
				// 권총이 맞은 Enemy에 데미지를 준다.
				if (HitActor->IsA(AEnemyZombie::StaticClass()))
				{
					// 데칼 생성
					FHitResult HitEnemy;
					FVector DecalStartLocation = HitResult.Location;
					FVector DecalEndLocation = DecalStartLocation + FVector(0, 0, 1) * -1000.0f;
					GetWorld()->LineTraceSingleByChannel(HitEnemy, DecalStartLocation, DecalEndLocation, ECC_Visibility, Params);
					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, FVector(-68, -128, -128), HitEnemy.Location, HitEnemy.ImpactNormal.Rotation());
					UE_LOG(LogTemp, Warning, TEXT("Decal"));

					// 권총에 적이 맞을때 파티클 생성
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEnemyParticle, HitResult.ImpactPoint, FRotator::ZeroRotator);
					AEnemyZombie* Enemy = Cast<AEnemyZombie>(HitActor);
					UEnemyFSM* EnemyFSM = Cast<UEnemyFSM>(Enemy->GetDefaultSubobjectByName(TEXT("FSM")));
					if (EnemyFSM)
					{
						if(EnemyFSM->isdying == false)
						{
							GameMode->PlayZedTime(0.5f);
							EnemyFSM->OnDamageProcess(1);
						}
					
					}
					//EnemyFSM->hp -= 1;
					//if (EnemyFSM->hp <= 0)
					//{
					//	GameMode->SubtractCurrentEnemyLeft();
					//	GameMode->CheckEnemyLeft();
					//	Enemy->Destroy();

					//}
				}
				// 권총이 맞은 Boss에 데미지를 준다.
				if (HitActor->IsA(ABoss::StaticClass()))
				{
				
					// 권총에 적이 맞을때 파티클 생성
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEnemyParticle, HitResult.ImpactPoint, FRotator::ZeroRotator);
					auto Boss = Cast<ABoss>(HitActor);
					UBossFSM* BossFSM = Cast<UBossFSM>(Boss->GetDefaultSubobjectByName(TEXT("FSM")));
					if(BossFSM)
					{
						if(BossFSM->isdying == false)
						{
							GameMode->PlayZedTime(0.5f);
							BossFSM->OnDamageProcess(1);
						}
					

						/*BossFSM->hp -= 1;
						if (BossFSM->hp <= 0)
						{
							GameMode->SubtractCurrentEnemyLeft();
							GameMode->CheckEnemyLeft();
							Boss->Destroy();
						}*/
					}
				}
			}

		 
		}
	}
	else
	{
		// 재장전 함수 실행
		ReloadTime();
	}
}

void AHandgun::ReloadPistol()
{	
	// 다시 탄약을 최대 탄약으로 채운다.
	HandgunAmmo = HandgunAmmoMax;

	bCanFire = true;

}

void AHandgun::ReloadTime()
{
	// 총 발사를 막는다
	if (bCanFire == false)
	{
		return;
	}
	bCanFire = false;

	// 재장전 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HandgunReloadSound, HandgunMesh->GetSocketLocation("PewPewPoint"));

	// 장전시간 3초
	FTimerHandle ReloadTimer;
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &AHandgun::ReloadPistol, 3.0f, false);

}

void AHandgun::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 플레이어가 권총을 획득했다면
	if (OtherActor->IsA(AVRPlayer::StaticClass()))
	{
		auto Player = Cast<AVRPlayer>(OtherActor);
		Player->handgunWeapon = this;

		Player->gunCount += 1;
		Player->mainUI->PrintGunCount();

		//Player->WeaponType = 0;
		// 권총을 플레이어의 손에 넣기
		HandgunMesh->AttachToComponent(Player->RightHand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHand"));
		// 권총을 플레이어의 손에 넣었으니 권총을 플레이어가 움직이지 않도록 설정
		HandgunMesh->SetSimulatePhysics(false);
		// 권총을 플레이어가 움직이지 않도록 설정했으니 권총의 충돌을 끄기
		HandgunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 권총을 플레이어가 움직이지 않도록 설정했으니 권총의 충돌을 끄기
		PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 핸드건 메쉬 크기설정
		HandgunMesh->SetRelativeScale3D(FVector(1.6f));
		// 먹으면 나오는 권총 위치
		HandgunMesh->SetRelativeLocationAndRotation(FVector(-0.76f, -0.54f, -11.6f), FRotator(-4.7f, 88.7f, -61.0f));
		//(X=-0.758474,Y=-0.543023,Z=-11.587390)  ,  (Pitch=-4.730560,Yaw=88.743119,Roll=-61.078881)
	}
}