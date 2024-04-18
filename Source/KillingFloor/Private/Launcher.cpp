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


	// 구체 콜리전 생성 및 설정 (픽업을 위한)
	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetSphereRadius(30.0f);
	PickupSphere->SetupAttachment(LauncherMesh);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	PickupSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// 라이플 메쉬 설정
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempLauncher(TEXT("/Script/Engine.StaticMesh'/Game/3DObject/Rocket/Grendade_Launcher.Grendade_Launcher'"));
	// 불러오는데 성공했다면..
	if (TempLauncher.Succeeded())
	{
		// 라이플 메쉬에 적용
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
	// 플레이어가 런처를 획득했다면
	if (OtherActor->IsA(AVRPlayer::StaticClass()))
	{
		auto Player = Cast<AVRPlayer>(OtherActor);
		Player->launcherWeapon = this;

		Player->gunCount += 1;
		Player->mainUI->PrintGunCount();
		//Player->WeaponType = 2;
		// 런처를 플레이어의 손에 넣기
		LauncherMesh->AttachToComponent(Player->RightHand, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RightHand"));
		// 런처를 플레이어의 손에 넣었으니 런처를 플레이어가 움직이지 않도록 설정
		LauncherMesh->SetSimulatePhysics(false);
		// 런처를 플레이어가 움직이지 않도록 설정했으니 런처의 충돌을 끄기
		LauncherMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// pickup 콜리전 없애기
		PickupSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 런처 메쉬 크기설정
		LauncherMesh->SetRelativeScale3D(FVector(0.022f));
		// 먹으면 나오는 런처 위치
		LauncherMesh->SetRelativeLocationAndRotation(FVector(2.6f, 0.96f, -1.4f), FRotator(12.4f, -86.1f, 59.2f));
		//(X=2.603335,Y=0.962842,Z=-1.412123)  ,  (Pitch=12.400000,Yaw=-86.098490,Roll=59.200001)
	}
}

void ALauncher::FireLauncher()
{
	// RocketProjectile 을 소켓 boom에서 발사
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, LauncherMesh->GetSocketLocation(TEXT("Boom")));
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), LauncherFireSound, LauncherMesh->GetSocketLocation(TEXT("Boom")));

	// RocketProjectile 을 소켓 boom에서 소환
	FTransform t = LauncherMesh->GetSocketTransform(TEXT("Boom"));
	/*t.SetRotation(FQuat(LauncherMesh->GetComponentRotation()));*/
	// boom 소켓을90도 돌려준다.
	auto Grenade = GetWorld()->SpawnActor<ARocketProjectile>(RocketProjectileClass, t);
	// Grenade 가 소환될때 X축을 90도 돌려준다.
	if (Grenade != nullptr)
	{
		Grenade->RocketMesh->SetRelativeRotation(FRotator(90, 0, 0));
	}
}
