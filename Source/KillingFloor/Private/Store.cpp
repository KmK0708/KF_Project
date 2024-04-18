// Fill out your copyright notice in the Description page of Project Settings.


#include "Store.h"
#include "Handgun.h"
#include "Rifle.h"
#include "Launcher.h"
#include "Components/BoxComponent.h"
#include "VRPlayer.h"


// Sets default values
AStore::AStore()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 스토어 메쉬 생성
	StoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StoreMesh"));
	// 루트컴포넌트
	RootComponent = StoreMesh;

	// 오버랩 박스 생성
	StoreOverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("StoreBoxCollision"));
	// 박스크기 설정
	StoreOverlapCollision->SetBoxExtent(FVector(100.0f, 15.0f, 100.0f));

	// 루트 컴포넌트
	StoreOverlapCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AStore::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStore::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	AVRPlayer* VRPlayer = Cast<AVRPlayer>(OtherActor);
	if (VRPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("im IN"));
		VRPlayer->bisShopIn = true;
	}
	
}

void AStore::BuyWeapon(FName WeaponName, float PlayerMoney)
{
	// 무기 이름에 따라 무기를 생성한다.


	// 무기 생성시 무기의 위치를 플레이어의 위치로 설정한다.
	
	// 플레이어의 돈을 무기의 값만큼 차감한다.
}

