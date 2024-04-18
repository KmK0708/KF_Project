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
	//���� �ݸ��� ����
	RocketCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RocketCollision"));
	RocketCollision->SetSphereRadius(5.0f);
	RocketCollision->SetupAttachment(RootComponent);
	RocketCollision->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	

	// ���� �ݸ����� ���ö󰡰� �ϱ�
	RocketCollision->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.0f));
	RocketCollision->CanCharacterStepUpOn = ECB_No;
	//���� �޽� ����
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(RocketCollision);


	ConstructorHelpers::FObjectFinder<UStaticMesh> TempRocket(TEXT("/Script/Engine.StaticMesh'/Game/3DObject/Rocket/VOG25.VOG25'"));
	// �ҷ����µ� �����ߴٸ�..
	if (TempRocket.Succeeded())
	{
		// ���� �޽��� ����
		RocketMesh->SetStaticMesh(TempRocket.Object);
	}
	//���� ������Ÿ�� �����Ʈ ����
	RocketMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("RocketProjectile"));
	RocketMovement->SetUpdatedComponent(RocketCollision);
	RocketMovement->InitialSpeed = 2000.0f;
	RocketMovement->MaxSpeed = 2000.0f;
	RocketMovement->bRotationFollowsVelocity = true;
	RocketMovement->bShouldBounce = true;
	RocketMovement->Bounciness = 0.3f;
	RocketMovement->ProjectileGravityScale = 1;

	// ������ ��, 3�ʵ� �ı�
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
	// ������ VRPlayer ī�޶� ����
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// ���� �̹� ���� �־��ٸ�
	if (ExcanShakeInstance != nullptr && ExcanShakeInstance->IsFinished() == false)
	{
		// ����ϰ�
		CameraManager->StopCameraShake(ExcanShakeInstance);
	}
	// ����ʹ�.
	ExcanShakeInstance = CameraManager->StartCameraShake(ExcameraShakeFactory);


		// ���߹��� ����
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
	// ���̶��
		if (isOverlapped)
		{
			UE_LOG(LogTemp, Warning, (TEXT("Hit")));
			// �߻�ü�� �����.
			RocketMovement->StopMovementImmediately();
			// ���� ����Ʈ ����
			UParticleSystemComponent* Explosion = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RocketParticle, GetActorTransform());
			// ���� ���
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), RocketBoomSound, GetActorLocation());
			for (AActor* OverlappedActor : outActors)
			{
				UE_LOG(LogTemp, Warning, TEXT("OverlappedActor: %s"), *OverlappedActor->GetName());

				AEnemyZombie* Enemy = Cast<AEnemyZombie>(OverlappedActor);
				if (Enemy != nullptr)
				{
					// ��Į ����
					FHitResult HitEnemy;
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(this);
					FVector DecalStartLocation = SweepResult.Location;
					FVector DecalEndLocation = DecalStartLocation + FVector(0, 0, 1) * -1000.0f;
					GetWorld()->LineTraceSingleByChannel(HitEnemy, DecalStartLocation, DecalEndLocation, ECC_Visibility, Params);
					UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, FVector(-68, -128, -128), HitEnemy.Location, HitEnemy.ImpactNormal.Rotation());

					// ������ ExplosionDamage�� ������.
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
					// ��Į ����
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


	// �߻�ü�� �ı��Ѵ�.
	Destroy();

}


//void ARocketProjectile::OnDetonate()
//{
//	UParticleSystemComponent* Explosion = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RocketParticle, GetActorTransform());
//	Explosion->SetRelativeScale3D(FVector(4.0f));
//
//	// ���� ���
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
