// Fill out your copyright notice in the Description page of Project Settings.


#include "NavigationalGuide.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "VRPlayer.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Quat.h"

// Sets default values
ANavigationalGuide::ANavigationalGuide()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	spline = CreateDefaultSubobject<USplineComponent>(TEXT("spline"));
	spline->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ANavigationalGuide::BeginPlay()
{
	Super::BeginPlay();

	
	// 1초마다 navigation guide 를 보여준다.
	//GetWorld()->GetTimerManager().SetTimer(navGuideTimerHandle, this, &ANavigationalGuide::BuildNavigation, 5.0f, true, 0);

	target = Cast<AVRPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	pathStartActorLocation = target->GetActorLocation();

	pathEndActor = this;

	BuildNavigation();
}

// Called every frame
void ANavigationalGuide::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANavigationalGuide::BuildNavigation()
{
	// spline, wayPointList clear
	ClearSpline();

	pathStartActorLocation = target->GetActorLocation();
	TArray<FVector> findPathPoints = FindPath(pathStartActorLocation, pathEndActor);
	rot = target->GetActorQuat();

	BuildSpline(findPathPoints);

	BuildPath();

	
}

TArray<FVector> ANavigationalGuide::FindPath(FVector pathStart, AActor* pathEnd)
{
	UNavigationPath* navPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(), pathStart, pathEnd);

	return navPath->PathPoints;
}

void ANavigationalGuide::BuildSpline(TArray<FVector>& pathPoints)
{
	for (int32 i = 0; i < pathPoints.Num(); i++)
	{
		spline->AddSplinePoint(pathPoints[i], ESplineCoordinateSpace::World, true);
	}
}

void ANavigationalGuide::BuildPath()
{
	GetWorldTimerManager().SetTimer(spawnTimer, FTimerDelegate::CreateLambda(
		[this]()->void
		{
			FVector loc = spline->GetLocationAtDistanceAlongSpline(arrowCount * 300, ESplineCoordinateSpace::World) + FVector(0, 0, 30);
			//FVector nxtLoc = spline->GetLocationAtDistanceAlongSpline((arrowCount + 1) * 300, ESplineCoordinateSpace::World) + FVector(0, 0, 50);

			FVector tangentLoc = spline->GetWorldTangentAtDistanceAlongSpline(arrowCount * 300);

			FQuat rotAngle = FQuat::FindBetween(target->GetActorForwardVector(), tangentLoc);

			FTransform trans;
			trans.SetLocation(loc);
			trans.SetRotation(rot);
			if (navGuideShapeActor)
			{
				AActor* arrow = GetWorld()->SpawnActor<AActor>(navGuideShapeActor, trans);
				arrow->AddActorLocalRotation(rotAngle);
				WayPointList.Add(arrow);
			}

			delayTime += 0.5f;
			arrowCount++;

			FVector endLoc = pathEndActor->GetActorLocation();
			if (FVector::Dist(FVector(loc.X, loc.Y, 0), FVector(endLoc.X, endLoc.Y, 0)) < 10.0f)
			{
				GetWorld()->GetTimerManager().SetTimer(navGuideTimerHandle, this, &ANavigationalGuide::BuildNavigation, 1.0f);

				GetWorldTimerManager().ClearTimer(spawnTimer);
					
			}
		}
	), 0.2f, true);

}

void ANavigationalGuide::ClearSpline()
{
	arrowCount = 0;
	delayTime = 0.0f;
	spline->ClearSplinePoints(true);

	/*for (int32 i = 0; i < WayPointList.Num(); i++)
	{
		WayPointList[i]->Destroy();
	}*/
	WayPointList.Empty();
}

