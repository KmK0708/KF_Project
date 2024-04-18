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
	// ���� ����� �޽�������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
		class UStaticMeshComponent* StoreMesh;
	// ���� ����� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
		class UBoxComponent* StoreOverlapCollision;

	// VRPlayer �� StoreOverlapCollision�ϰ� ������ �Ǿ�����.
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Store")
	// ������������ �÷��̾ �ִ���
	bool bIsPlayerInStore;

	// ���� ���� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Store")
		void BuyWeapon(FName WeaponName, float PlayerMoney);
};
