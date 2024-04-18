// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Store.generated.h"

UCLASS()
class KILLINGFLOOR_API AStore : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStore();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 무기 스토어 메쉬컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
		class UStaticMeshComponent* StoreMesh;
	// 무기 스토어 오버랩
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
		class UBoxComponent* StoreOverlapCollision;

	// VRPlayer 가 StoreOverlapCollision하고 오버랩 되었을때.
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
	// 상점구역에서 플레이어가 있는지
	bool bIsPlayerInStore;

	// 무기 구매 함수
	UFUNCTION(BlueprintCallable, Category = "Store")
		void BuyWeapon(FName WeaponName, float PlayerMoney);
};
