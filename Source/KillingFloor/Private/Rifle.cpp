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

	// ��ü �ݸ��� ���� �� ���� (�Ⱦ��� ����)
	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetSphereRadius(50.0f);
	PickupSphere->SetupAttachment(RifleMesh);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	PickupSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// ������ �޽� ����
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempRifle(TEXT("/Script/Engine.StaticMesh'/Game/3DObject/Rifle/M16.M16'"));
	// �ҷ����µ� �����ߴٸ�..
	if (TempRifle.Succeeded())
	{
		// ������ �޽��� ����
		RifleMesh->SetStaticMesh(TempRifle.Object);
	}

	// ����ƽ�޽��� ������ Socket bang�� ã�Ƽ� MuzzleFireNiagara�� ���̱�
	RifleMesh->GetSocketLocation("bang");
	MuzzleFireNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleFireNiagara"));
	// MuzzleFireniagara�� socket bang�� ���̱�
	MuzzleFireNiagara->SetupAttachment(RifleMesh, "bang");
	MuzzleFireNiagara->bAutoActivate = false;

	bIsPickedUp = false;
	// ó�� ź�� �� �ִ� ź��� ����.
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
	// �÷��̾ ������ ȹ���ߴٸ�
	if (OtherActor->IsA(AVRPlayer::StaticClass()))
	{
		auto Player = Cast<AVRPlayer>(OtherActor);
		Player->rifleWeapon = this;

		Player->gunCount += 1;
		Player->mainUI->PrintGunCount();

		//Player->WeaponType = 1;
		// ���� �÷��̾��� �տ� �ֱ�
		RifleMesh->AttachToComponent(Player->RightHand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHand"));
		// ���� �÷��̾��� �տ� �־����� ���� �÷��̾ �������� �ʵ��� ����
		RifleMesh->SetSimulatePhysics(false);
		// ���� �÷��̾ �������� �ʵ��� ���������� ���� �浹�� ����
		RifleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// ���� �÷��̾ �������� �ʵ��� ���������� ���� �浹�� ����
		PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// �� �޽� ũ�⼳��
		RifleMesh->SetRelativeScale3D(FVector(1));
		// ������ ������ �� ��ġ
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
		// ���� �߻�
		FVector StartLocation = RifleMesh->GetSocketLocation("bang");
		FVector ForwardVector = RifleMesh->GetForwardVector();		// ��������:���� ���������� ������ ����
		FVector EndLocation = StartLocation + (ForwardVector.RotateAngleAxis(-90.f, RifleMesh->GetUpVector()) * 10000.0f);
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
	
		// �� �߻� ���� ���
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), RifleFireSound, StartLocation);

		// ������ �Ƹ� 1�� ����
		RifleAmmo--;

		// ���� �߻�
		GetWorldTimerManager().SetTimer(FireTimer, this, &ARifle::FireRifle, FireRate, true);

		// ī�޶� ����ʹ�.
		APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		RiflecanShakeInstance = cameraManager->StartCameraShake(RiflecameraShakeFactory);


		// ���̾ư��� �ѱ�ȭ���� ���ϳ��� bang�� ����
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFire, StartLocation, FRotator::ZeroRotator);
		// �ѱ�ȭ�� �ѹ������� �ı�
		MuzzleFireNiagara->Deactivate();

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params))
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				// ���� ���� ��ġ�� ��ƼŬ ����
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitResult.ImpactPoint, FRotator::ZeroRotator);

				// ���� ���� Enemy�� �������� �ش�.
				if (HitActor->IsA(AEnemyZombie::StaticClass()))
				{
					// ��Į ����
					FHitResult HitEnemy;
					FVector DecalStartLocation = HitResult.Location;
					FVector DecalEndLocation = DecalStartLocation + FVector(0,0,1) * -1000.0f;
					GetWorld()->LineTraceSingleByChannel(HitEnemy, DecalStartLocation, DecalEndLocation, ECC_Visibility, Params);
					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, FVector(-68, -128, -128), HitEnemy.Location, HitEnemy.ImpactNormal.Rotation());
					UE_LOG(LogTemp, Warning, TEXT("Decal"));
					//// �� ��Į ���͸� �����մϴ�.
					//ADecalActor* BloodDecal = GetWorld()->SpawnActor<ADecalActor>(BloodDecalClass);

					//// �� ��Į ������ ��ġ�� ȸ���� �����մϴ�.
					//BloodDecal->SetActorLocation(HitResult.Location);
					//BloodDecal->SetActorRotation(HitResult.Normal.Rotation() + FRotator(0.0f, 180.0f, 0.0f));

					// �ѿ� ���� �¾����� ��ƼŬ ����
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
					// �ѿ� ���� �¾����� ��ƼŬ ����
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
		// ������ �Լ� ����
		ReloadTime();
	}
}

void ARifle::ReloadRifle()
{
	// �ٽ� ź���� �ִ� ź������ ä���.
	RifleAmmo = RifleAmmoMax;

	bCanFire = true;
}

void ARifle::ReloadTime()
{
	// �� �߻縦 ���´�
	if (bCanFire == false)
	{
		return;
	}
	bCanFire = false;

	// ������ ���� ���
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), RifleReloadSound, RifleMesh->GetSocketLocation("bang"));

	// �����ð� 3��
	FTimerHandle ReloadTimer;
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &ARifle::ReloadRifle, 3.0f, false);
}