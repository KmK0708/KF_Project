// Fill out your copyright notice in the Description page of Project Settings.


#include "StartVRPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include <Camera/CameraComponent.h>
#include <MotionControllerComponent.h>
#include <HeadMountedDisplayFunctionLibrary.h>
#include <UMG/Public/Components/WidgetInteractionComponent.h>

// Sets default values
AStartVRPlayer::AStartVRPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(RootComponent);
	VRCamera->bUsePawnControlRotation = false;

	// ���߰�
	LeftHand = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHand"));
	LeftHand->SetupAttachment(RootComponent);
	LeftHand->SetTrackingMotionSource(FName("Left"));

	RightHand = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHand"));
	RightHand->SetupAttachment(RootComponent);
	RightHand->SetTrackingMotionSource(FName("Right"));

	// ���̷�Ż�޽�������Ʈ �����
	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandMesh"));
	LeftHandMesh->SetupAttachment(LeftHand);
	// ���̷�Ż�޽� �ε��ؼ� �Ҵ�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_left.SKM_MannyXR_left'"));
	if (TempMesh.Succeeded())
	{
		LeftHandMesh->SetSkeletalMesh(TempMesh.Object);
		LeftHandMesh->SetRelativeLocation(FVector(-2.9f, -3.5f, 4.5f));
		LeftHandMesh->SetRelativeRotation(FRotator(-25, -180, 90));
	}

	// ������ �޽�
	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandMesh"));
	RightHandMesh->SetupAttachment(RightHand);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh2(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_right.SKM_MannyXR_right'"));
	if (TempMesh2.Succeeded())
	{
		RightHandMesh->SetSkeletalMesh(TempMesh2.Object);
		RightHandMesh->SetRelativeLocation(FVector(-2.9f, 3.5f, 4.5f));
		RightHandMesh->SetRelativeRotation(FRotator(25, 0, 90));
	}

	// ���Լհ���
	RightAim = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightAim"));
	RightAim->SetupAttachment(RootComponent);
	RightAim->SetTrackingMotionSource(FName("RightAim"));

	WidgetInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionComp"));
	WidgetInteractionComp->SetupAttachment(RightAim);
}

// Called when the game starts or when spawned
void AStartVRPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// Enhanced Input ���ó��
	auto PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

	if (PC)
	{
		// LocalPlayer
		auto localPlayer = PC->GetLocalPlayer();
		auto subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(localPlayer);
		if (subSystem)
		{
			subSystem->AddMappingContext(IMC_VRInput, 0);
			subSystem->AddMappingContext(IMC_Hand, 0);
		}
	}

	// HMD �� ����Ǿ� ���� �ʴٸ�
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
	{
		// Hand �� �׽�Ʈ �� �� �ִ� ��ġ�� �̵���Ű��.
		RightHand->SetRelativeLocation(FVector(20, 20, 0));
		RightAim->SetRelativeLocation(FVector(20, 20, 0));
		// ī�޶��� UsePawn Control Rotation �� Ȱ��ȭ ��Ű��
		VRCamera->bUsePawnControlRotation = true;
	}
	// ���� HMD �� ����Ǿ� �ִٸ�
	else
	{
		// -> �⺻ Ʈ��ŷ offset ����
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
	}
}

// Called every frame
void AStartVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// HMD �� ����� ���� ������
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
	{
		// -> ���� ī�޶� ����� ��ġ�ϵ��� ����
		RightHand->SetRelativeRotation(VRCamera->GetRelativeRotation());
		RightAim->SetRelativeRotation(VRCamera->GetRelativeRotation());
	}

}

// Called to bind functionality to input
void AStartVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto InputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (InputSystem)
	{
		InputSystem->BindAction(IA_Mouse, ETriggerEvent::Triggered, this, &AStartVRPlayer::Turn);

		InputSystem->BindAction(IA_Fire, ETriggerEvent::Started, this, &AStartVRPlayer::FireInput);
		InputSystem->BindAction(IA_Fire, ETriggerEvent::Completed, this, &AStartVRPlayer::ReleaseUIInput);
	}
}

void AStartVRPlayer::Turn(const FInputActionValue& Values)
{
	FVector2D Axis = Values.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AStartVRPlayer::FireInput(const FInputActionValue& Values)
{
	// UI �� �̺�Ʈ �����ϰ� �ʹ�.
	if (WidgetInteractionComp)
	{
		WidgetInteractionComp->PressPointerKey(EKeys::LeftMouseButton);
	}

}

void AStartVRPlayer::ReleaseUIInput()
{
	if (WidgetInteractionComp)
	{
		WidgetInteractionComp->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}

