
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
	

	// ��ü �ݸ��� ���� �� ���� (�Ⱦ��� ����)
	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetSphereRadius(50.0f);
	PickupSphere->SetupAttachment(HandgunMesh);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	PickupSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// ���� �޽� ����
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempPistol(TEXT("/Script/Engine.StaticMesh'/Game/3DObject/Pistol/M1911.M1911'"));
	// �ҷ����µ� �����ߴٸ�..
	if (TempPistol.Succeeded())
	{
		// ���� �޽��� ����
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
	// ó�� ź�� �� �ִ� ź��� ����.
	HandgunAmmo = HandgunAmmoMax;
}

// Called every frame
void AHandgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHandgun::FirePistol()
{
	// ���� ź���� 0���� ũ�ٸ�
	if (HandgunAmmo > 0)
	{
		bCanFire = true;
		UE_LOG(LogTemp, Warning, TEXT("FirePistol"));
		// ���� �߻�
		FVector StartLocation = HandgunMesh->GetSocketLocation("PewPewPoint");
		FVector ForwardVector = HandgunMesh->GetForwardVector();		// ��������:���� ���������� ������ ����
		FVector EndLocation = StartLocation + (ForwardVector.RotateAngleAxis(-90.f, HandgunMesh->GetUpVector()) * 10000.0f);
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
		// �� �� �ѱ� �տ��� ȭ���߻�
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, HandgunMesh->GetSocketLocation("PewPewPoint"));

		// �� �߻� ���� ���
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HandgunFireSound, HandgunMesh->GetSocketLocation("PewPewPoint"));

		// ź���� 1�� �Ҹ�
		HandgunAmmo--;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params))
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				// ������ ���� ��ġ�� ��ƼŬ ����
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitResult.ImpactPoint, FRotator::ZeroRotator);
				// ������ ���� Enemy�� �������� �ش�.
				if (HitActor->IsA(AEnemyZombie::StaticClass()))
				{
					// ��Į ����
					FHitResult HitEnemy;
					FVector DecalStartLocation = HitResult.Location;
					FVector DecalEndLocation = DecalStartLocation + FVector(0, 0, 1) * -1000.0f;
					GetWorld()->LineTraceSingleByChannel(HitEnemy, DecalStartLocation, DecalEndLocation, ECC_Visibility, Params);
					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, FVector(-68, -128, -128), HitEnemy.Location, HitEnemy.ImpactNormal.Rotation());
					UE_LOG(LogTemp, Warning, TEXT("Decal"));

					// ���ѿ� ���� ������ ��ƼŬ ����
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
				// ������ ���� Boss�� �������� �ش�.
				if (HitActor->IsA(ABoss::StaticClass()))
				{
				
					// ���ѿ� ���� ������ ��ƼŬ ����
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
		// ������ �Լ� ����
		ReloadTime();
	}
}

void AHandgun::ReloadPistol()
{	
	// �ٽ� ź���� �ִ� ź������ ä���.
	HandgunAmmo = HandgunAmmoMax;

	bCanFire = true;

}

void AHandgun::ReloadTime()
{
	// �� �߻縦 ���´�
	if (bCanFire == false)
	{
		return;
	}
	bCanFire = false;

	// ������ ���� ���
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HandgunReloadSound, HandgunMesh->GetSocketLocation("PewPewPoint"));

	// �����ð� 3��
	FTimerHandle ReloadTimer;
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &AHandgun::ReloadPistol, 3.0f, false);

}

void AHandgun::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// �÷��̾ ������ ȹ���ߴٸ�
	if (OtherActor->IsA(AVRPlayer::StaticClass()))
	{
		auto Player = Cast<AVRPlayer>(OtherActor);
		Player->handgunWeapon = this;

		Player->gunCount += 1;
		Player->mainUI->PrintGunCount();

		//Player->WeaponType = 0;
		// ������ �÷��̾��� �տ� �ֱ�
		HandgunMesh->AttachToComponent(Player->RightHand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHand"));
		// ������ �÷��̾��� �տ� �־����� ������ �÷��̾ �������� �ʵ��� ����
		HandgunMesh->SetSimulatePhysics(false);
		// ������ �÷��̾ �������� �ʵ��� ���������� ������ �浹�� ����
		HandgunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// ������ �÷��̾ �������� �ʵ��� ���������� ������ �浹�� ����
		PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// �ڵ�� �޽� ũ�⼳��
		HandgunMesh->SetRelativeScale3D(FVector(1.6f));
		// ������ ������ ���� ��ġ
		HandgunMesh->SetRelativeLocationAndRotation(FVector(-0.76f, -0.54f, -11.6f), FRotator(-4.7f, 88.7f, -61.0f));
		//(X=-0.758474,Y=-0.543023,Z=-11.587390)  ,  (Pitch=-4.730560,Yaw=88.743119,Roll=-61.078881)
	}
}