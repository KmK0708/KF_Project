// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigationalGuide.generated.h"

UCLASS()
class KILLINGFLOOR_API ANavigationalGuide : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANavigationalGuide();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
	class USplineComponent* spline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
	FVector pathStartActorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
	AActor* pathEndActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
	TSubclassOf<class AActor> navGuideShapeActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
	class UStaticMeshComponent* meshComp;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
	//	TSubclassOf<class UStaticMeshComponent> staticMeshComp;

private:
	UPROPERTY()
	FTimerHandle navGuideTimerHandle;

	UPROPERTY()
	TArray<AActor*> WayPointList;

	UPROPERTY()
	class AVRPlayer* target;

	FQuat rot;
	FTimerHandle spawnTimer;
	int32 arrowCount = 0;
	float delayTime = 0.0f;

	UFUNCTION()
	void BuildNavigation();

	UFUNCTION()
	TArray<FVector> FindPath(FVector pathStart, AActor* pathEnd);

	UFUNCTION()
	void BuildSpline(TArray<FVector>& pathPoints);

	UFUNCTION()
	void BuildPath();

	UFUNCTION()
	void ClearSpline();

};
