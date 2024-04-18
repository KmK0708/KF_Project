// Fill out your copyright notice in the Description page of Project Settings.


#include "Launcher.h"
#include <Kismet/GameplayStatics.h>
#include <Components/SphereComponent.h>
#include "RocketProjectile.h"
#include "DrawDebugHelpers.h"
#include "VRPlayer.h"
#include "EnemyZombie.h"
#include "EnemyFSM.h"
#include "MotionControllerComponent.h"
#include "KillingFloorGameModeBase.h"
#include "PlayerMainWidget.h"

// Sets default values
ALauncher::ALauncher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LauncherMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LauncherMesh"));
	SetRootComponent(LauncherMesh);


	// ��ü �ݸ��� ���� �� ���� (�Ⱦ��� ����)
	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetSphereRadius(30.0f);
	PickupSphere->SetupAttachment(LauncherMesh);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	PickupSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// ������ �޽� ����
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempLauncher(TEXT("/Script/Engine.StaticMesh'/Game/3DObject/Rocket/Grendade_Launcher.Grendade_Launcher'"));
	// �ҷ����µ� �����ߴٸ�..
	if (TempLauncher.Succeeded())
	{
		// ������ �޽��� ����
		LauncherMesh->SetStaticMesh(TempLauncher.Object);
	}

	bIsPickedUp = false;
}

// Called when the game starts or when spawned
void ALauncher::BeginPlay()
{
	Super::BeginPlay();
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &ALauncher::OnOverlapBegin);
}

// Called every frame
void ALauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALauncher::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// �÷��̾ ��ó�� ȹ���ߴٸ�
	if (OtherActor->IsA(AVRPlayer::StaticClass()))
	{
		auto Player = Cast<AVRPlayer>(OtherActor);
		Player->launcherWeapon = this;

		Player->gunCount += 1;
		Player->mainUI->PrintGunCount();
		//Player->WeaponType = 2;
		// ��ó�� �÷��̾��� �տ� �ֱ�
		LauncherMesh->AttachToComponent(Player->RightHand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHand"));
		// ��ó�� �÷��̾��� �տ� �־����� ��ó�� �÷��̾ �������� �ʵ��� ����
		LauncherMesh->SetSimulatePhysics(false);
		// ��ó�� �÷��̾ �������� �ʵ��� ���������� ��ó�� �浹�� ����
		LauncherMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// pickup �ݸ��� ���ֱ�
		PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// ��ó �޽� ũ�⼳��
		LauncherMesh->SetRelativeScale3D(FVector(0.022f));
		// ������ ������ ��ó ��ġ
		LauncherMesh->SetRelativeLocationAndRotation(FVector(2.6f, 0.96f, -1.4f), FRotator(12.4f, -86.1f, 59.2f));
		//(X=2.603335,Y=0.962842,Z=-1.412123)  ,  (Pitch=12.400000,Yaw=-86.098490,Roll=59.200001)
	}
}

void ALauncher::FireLauncher()
{
	// RocketProjectile �� ���� boom���� �߻�
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, LauncherMesh->GetSocketLocation(TEXT("Boom")));
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), LauncherFireSound, LauncherMesh->GetSocketLocation(TEXT("Boom")));

	// RocketProjectile �� ���� boom���� ��ȯ
	FTransform t = LauncherMesh->GetSocketTransform(TEXT("Boom"));
	/*t.SetRotation(FQuat(LauncherMesh->GetComponentRotation()));*/
	// boom ������90�� �����ش�.
	auto Grenade = GetWorld()->SpawnActor<ARocketProjectile>(RocketProjectileClass, t);
	// Grenade �� ��ȯ�ɶ� X���� 90�� �����ش�.
	if (Grenade != nullptr)
	{
		Grenade->RocketMesh->SetRelativeRotation(FRotator(90, 0, 0));
	}
}
