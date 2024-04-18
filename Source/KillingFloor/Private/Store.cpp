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

	// ����� �޽� ����
	StoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StoreMesh"));
	// ��Ʈ������Ʈ
	RootComponent = StoreMesh;

	// ������ �ڽ� ����
	StoreOverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("StoreBoxCollision"));
	// �ڽ�ũ�� ����
	StoreOverlapCollision->SetBoxExtent(FVector(100.0f, 15.0f, 100.0f));

	// ��Ʈ ������Ʈ
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
	// ���� �̸��� ���� ���⸦ �����Ѵ�.


	// ���� ������ ������ ��ġ�� �÷��̾��� ��ġ�� �����Ѵ�.
	
	// �÷��̾��� ���� ������ ����ŭ �����Ѵ�.
}

