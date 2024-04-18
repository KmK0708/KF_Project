// Fill out your copyright notice in the Description page of Project Settings.


#include "Rifle.h"
#include <Kismet/GameplayStatics.h>
#include <Components/SphereComponent.h>
#include "DrawDebugHelpers.h"
#include "VRPlayer.h"
#include "Camera/CameraComponent.h"
#include "EnemyZombie.h"
#include "EnemyFSM.h"
#include "PlayerMainWidget.h"
#include "Boss.h"
#include "BossFSM.h"
#include "KillingFloorGameModeBase.h"
#include "MotionControllerComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/DecalActor.h"
#include <NiagaraComponent.h>
#include <Niagara/Public/NiagaraFunctionLibrary.h>

// Sets default values
ARifle::ARifle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RifleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RifleMesh"));
	SetRootComponent(RifleMesh);

	// 구체 콜리전 생성 및 설정 (픽업을 위한)
	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetSphereRadius(50.0f);
	PickupSphere->SetupAttachment(RifleMesh);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	PickupSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// 라이플 메쉬 설정
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempRifle(TEXT("/Script/Engine.StaticMesh'/Game/3DObject/Rifle/M16.M16'"));
	// 불러오는데 성공했다면..
	if (TempRifle.Succeeded())
	{
		// 라이플 메쉬에 적용
		RifleMesh->SetStaticMesh(TempRifle.Object);
	}

	// 스태틱메쉬에 생성된 Socket bang을 찾아서 MuzzleFireNiagara에 붙이기
	RifleMesh->GetSocketLocation("bang");
	MuzzleFireNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleFireNiagara"));
	// MuzzleFireniagara를 socket bang에 붙이기
	MuzzleFireNiagara->SetupAttachment(RifleMesh, "bang");
	MuzzleFireNiagara->bAutoActivate = false;

	bIsPickedUp = false;
	// 처음 탄약 이 최대 탄약과 같다.
	RifleAmmo = RifleAmmoMax;
}

// Called when the game starts or when spawned
void ARifle::BeginPlay()
{
	Super::BeginPlay();
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &ARifle::OnOverlapBegin);
	GameMode = Cast<AKillingFloorGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
}

// Called every frame
void ARifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ARifle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 플레이어가 권총을 획득했다면
	if (OtherActor->IsA(AVRPlayer::StaticClass()))
	{
		auto Player = Cast<AVRPlayer>(OtherActor);
		Player->rifleWeapon = this;

		Player->gunCount += 1;
		Player->mainUI->PrintGunCount();

		//Player->WeaponType = 1;
		// 총을 플레이어의 손에 넣기
		RifleMesh->AttachToComponent(Player->RightHand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHand"));
		// 총을 플레이어의 손에 넣었으니 총을 플레이어가 움직이지 않도록 설정
		RifleMesh->SetSimulatePhysics(false);
		// 총을 플레이어가 움직이지 않도록 설정했으니 총의 충돌을 끄기
		RifleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 총을 플레이어가 움직이지 않도록 설정했으니 총의 충돌을 끄기
		PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 건 메쉬 크기설정
		RifleMesh->SetRelativeScale3D(FVector(1));
		// 먹으면 나오는 총 위치
		RifleMesh->SetRelativeLocationAndRotation(FVector(5.04f, -5.79f, -28.2f), FRotator(-13.20f, 91.8f, -55.68f));
		//(X=5.048167,Y=-5.796779,Z=-28.267858)  , (Pitch=-13.200000,Yaw=91.800003,Roll=-55.680000)
		RifleMesh->SetVisibility(false);
	}
}

void ARifle::FireRifle()
{
	if (RifleAmmo > 0)
	{
		bCanFire = true;
		UE_LOG(LogTemp, Warning, TEXT("FireRifle"));
		// 권총 발사
		FVector StartLocation = RifleMesh->GetSocketLocation("bang");
		FVector ForwardVector = RifleMesh->GetForwardVector();		// 돌린이유:총이 오른쪽으로 꺾여서 나감
		FVector EndLocation = StartLocation + (ForwardVector.RotateAngleAxis(-90.f, RifleMesh->GetUpVector()) * 10000.0f);
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
	
		// 총 발사 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), RifleFireSound, StartLocation);

		// 라이플 아모 1개 감소
		RifleAmmo--;

		// 연속 발사
		GetWorldTimerManager().SetTimer(FireTimer, this, &ARifle::FireRifle, FireRate, true);

		// 카메라를 흔들고싶다.
		APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		RiflecanShakeInstance = cameraManager->StartCameraShake(RiflecameraShakeFactory);


		// 나이아가라 총구화염을 소켓네임 bang에 생성
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFire, StartLocation, FRotator::ZeroRotator);
		// 총구화염 한번생성후 파괴
		MuzzleFireNiagara->Deactivate();

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params))
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				// 총이 맞은 위치에 파티클 생성
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitResult.ImpactPoint, FRotator::ZeroRotator);

				// 총이 맞은 Enemy에 데미지를 준다.
				if (HitActor->IsA(AEnemyZombie::StaticClass()))
				{
					// 데칼 생성
					FHitResult HitEnemy;
					FVector DecalStartLocation = HitResult.Location;
					FVector DecalEndLocation = DecalStartLocation + FVector(0,0,1) * -1000.0f;
					GetWorld()->LineTraceSingleByChannel(HitEnemy, DecalStartLocation, DecalEndLocation, ECC_Visibility, Params);
					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, FVector(-68, -128, -128), HitEnemy.Location, HitEnemy.ImpactNormal.Rotation());
					UE_LOG(LogTemp, Warning, TEXT("Decal"));
					//// 피 데칼 액터를 생성합니다.
					//ADecalActor* BloodDecal = GetWorld()->SpawnActor<ADecalActor>(BloodDecalClass);

					//// 피 데칼 액터의 위치와 회전을 설정합니다.
					//BloodDecal->SetActorLocation(HitResult.Location);
					//BloodDecal->SetActorRotation(HitResult.Normal.Rotation() + FRotator(0.0f, 180.0f, 0.0f));

					// 총에 적이 맞았을때 파티클 생성
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());

					AEnemyZombie* Enemy = Cast<AEnemyZombie>(HitActor);
					UEnemyFSM* EnemyFSM = Cast<UEnemyFSM>(Enemy->GetDefaultSubobjectByName(TEXT("FSM")));
					if (EnemyFSM)
					{
						if (EnemyFSM->isdying == false)
						{
							GameMode->PlayZedTime(0.5f);
							EnemyFSM->OnDamageProcess(2);
						}

					}


				}

				if (HitActor->IsA(ABoss::StaticClass()))
				{
					// 총에 적이 맞았을때 파티클 생성
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
					auto Boss = Cast<ABoss>(HitActor);
					UBossFSM* BossFSM = Cast<UBossFSM>(Boss->GetDefaultSubobjectByName(TEXT("FSM")));
					if (BossFSM)
					{
						if (BossFSM->isdying == false)
						{
							GameMode->PlayZedTime(0.5f);
							BossFSM->OnDamageProcess(2);
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

void ARifle::ReloadRifle()
{
	// 다시 탄약을 최대 탄약으로 채운다.
	RifleAmmo = RifleAmmoMax;

	bCanFire = true;
}

void ARifle::ReloadTime()
{
	// 총 발사를 막는다
	if (bCanFire == false)
	{
		return;
	}
	bCanFire = false;

	// 재장전 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), RifleReloadSound, RifleMesh->GetSocketLocation("bang"));

	// 장전시간 3초
	FTimerHandle ReloadTimer;
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &ARifle::ReloadRifle, 3.0f, false);
}