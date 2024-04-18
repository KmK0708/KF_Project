// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"
#include "Launcher.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "EnemyZombie.h"
#include "Particles/ParticleSystemComponent.h"
#include "EnemyFSM.h"
#include "Boss.h"
#include "BossFSM.h"
#include "Components/DecalComponent.h"
#include "Engine/DecalActor.h"
#include "KillingFloorGameModeBase.h"
#include "VRPlayer.h"


// Sets default values
ARocketProjectile::ARocketProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//로켓 콜리전 설정
	RocketCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RocketCollision"));
	RocketCollision->SetSphereRadius(5.0f);
	RocketCollision->SetupAttachment(RootComponent);
	RocketCollision->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	

	// 적이 콜리전에 못올라가게 하기
	RocketCollision->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
	RocketCollision->CanCharacterStepUpOn = ECB_No;
	//로켓 메쉬 설정
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(RocketCollision);


	ConstructorHelpers::FObjectFinder<UStaticMesh> TempRocket(TEXT("/Script/Engine.StaticMesh'/Game/3DObject/Rocket/VOG25.VOG25'"));
	// 불러오는데 성공했다면..
	if (TempRocket.Succeeded())
	{
		// 로켓 메쉬에 적용
		RocketMesh->SetStaticMesh(TempRocket.Object);
	}
	//로켓 프로젝타일 무브먼트 설정
	RocketMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("RocketProjectile"));
	RocketMovement->SetUpdatedComponent(RocketCollision);
	RocketMovement->InitialSpeed = 2000.0f;
	RocketMovement->MaxSpeed = 2000.0f;
	RocketMovement->bRotationFollowsVelocity = true;
	RocketMovement->bShouldBounce = true;
	RocketMovement->Bounciness = 0.3f;
	RocketMovement->ProjectileGravityScale = 1;

	// 생성된 뒤, 3초뒤 파괴
	InitialLifeSpan = 4.0f;

}

// Called when the game starts or when spawned
void ARocketProjectile::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<AKillingFloorGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	RocketCollision->OnComponentBeginOverlap.AddDynamic(this, &ARocketProjectile::OnOverlapBegin);
	
	RocketCollision->SetGenerateOverlapEvents(true);

}


// Called every frame
void ARocketProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARocketProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 터질때 VRPlayer 카메라 흔들기
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// 만약 이미 흔들고 있었다면
	if (ExcanShakeInstance != nullptr && ExcanShakeInstance->IsFinished() == false)
	{
		// 취소하고
		CameraManager->StopCameraShake(ExcanShakeInstance);
	}
	// 흔들고싶다.
	ExcanShakeInstance = CameraManager->StartCameraShake(ExcameraShakeFactory);


		// 폭발범위 생성
		TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
		traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel3));
		traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

		TArray<AActor*> ignoreActors;
		ignoreActors.Add(this);

		TArray<AActor*> outActors;
		

		float radius = 500.0f;
		FVector sphereSpawnLocation = GetActorLocation();
		bool isOverlapped = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), sphereSpawnLocation, radius, traceObjectTypes, nullptr, ignoreActors, outActors);
	// 적이라면
		if (isOverlapped)
		{
			UE_LOG(LogTemp, Warning, (TEXT("Hit")));
			// 발사체를 멈춘다.
			RocketMovement->StopMovementImmediately();
			// 폭발 이펙트 생성
			UParticleSystemComponent* Explosion = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RocketParticle, GetActorTransform());
			// 사운드 재생
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), RocketBoomSound, GetActorLocation());
			for (AActor* OverlappedActor : outActors)
			{
				UE_LOG(LogTemp, Warning, TEXT("OverlappedActor: %s"), *OverlappedActor->GetName());

				AEnemyZombie* Enemy = Cast<AEnemyZombie>(OverlappedActor);
				if (Enemy != nullptr)
				{
					// 데칼 생성
					FHitResult HitEnemy;
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(this);
					FVector DecalStartLocation = SweepResult.Location;
					FVector DecalEndLocation = DecalStartLocation + FVector(0, 0, 1) * -1000.0f;
					GetWorld()->LineTraceSingleByChannel(HitEnemy, DecalStartLocation, DecalEndLocation, ECC_Visibility, Params);
					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, FVector(-68, -128, -128), HitEnemy.Location, HitEnemy.ImpactNormal.Rotation());

					// 적에게 ExplosionDamage를 입힌다.
					UEnemyFSM* EnemyFSM = Cast<UEnemyFSM>(Enemy->GetDefaultSubobjectByName(TEXT("FSM")));
					if (EnemyFSM)
					{
						if (EnemyFSM->isdying == false)
						{
							GameMode->PlayZedTime(0.5f);
							EnemyFSM->OnDamageProcess(ExplosionDamage);
						}
					}
				}
				ABoss* Boss = Cast<ABoss>(OverlappedActor);
				if (Boss != nullptr)
				{
					// 데칼 생성
					FHitResult HitEnemy;
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(this);
					FVector DecalStartLocation = SweepResult.Location;
					FVector DecalEndLocation = DecalStartLocation + FVector(0, 0, 1) * -1000.0f;
					GetWorld()->LineTraceSingleByChannel(HitEnemy, DecalStartLocation, DecalEndLocation, ECC_Visibility, Params);
					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, FVector(-68, -128, -128), HitEnemy.Location, HitEnemy.ImpactNormal.Rotation());
					UBossFSM* BossFSM = Cast<UBossFSM>(Boss->GetDefaultSubobjectByName(TEXT("FSM")));
					if (BossFSM)
					{
						if (BossFSM->isdying == false)
						{
							GameMode->PlayZedTime(0.5f);
							BossFSM->OnDamageProcess(ExplosionDamage);
						}
					}
				}
			}
		}


	// 발사체를 파괴한다.
	Destroy();

}


//void ARocketProjectile::OnDetonate()
//{
//	UParticleSystemComponent* Explosion = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RocketParticle, GetActorTransform());
//	Explosion->SetRelativeScale3D(FVector(4.0f));
//
//	// 사운드 재생
//	UGameplayStatics::PlaySoundAtLocation(GetWorld(), RocketBoomSound, GetActorLocation());
//	
//	TArray<FHitResult> HitActors;
//
//	FVector StartTrace = GetActorLocation();
//	FVector EndTrace = StartTrace;
//	EndTrace.Z += 300.0f;
//
//	FCollisionShape CollisionShape;
//	CollisionShape.ShapeType = ECollisionShape::Sphere;
//	CollisionShape.SetSphere(ExplosionRadius);
//
//	if (GetWorld()->SweepMultiByChannel(HitActors, StartTrace, EndTrace, FQuat::Identity, ECC_Visibility, CollisionShape))
//	{
//		
//	}
//
//}
