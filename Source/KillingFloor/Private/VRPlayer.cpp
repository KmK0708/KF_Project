// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Handgun.h"
#include "Rifle.h"
#include "Launcher.h"
#include <HeadMountedDisplayFunctionLibrary.h>
#include "Components/WidgetComponent.h"
#include "KillingFloorGameModeBase.h"
#include "PlayerMainWidget.h"
#include <UMG/Public/Components/WidgetInteractionComponent.h>
#include "Store.h"


// Sets default values
AVRPlayer::AVRPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(RootComponent);
	
	// 손추가
	LeftHand = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHand"));
	LeftHand->SetupAttachment(RootComponent);
	LeftHand->SetTrackingMotionSource(FName("Left"));
	RightHand = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHand"));
	RightHand->SetupAttachment(RootComponent);
	RightHand->SetTrackingMotionSource(FName("Right"));

	// 왼손 메시
	// 스켈레탈메시컴포넌트 만들기
	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandMesh"));
	LeftHandMesh->SetupAttachment(LeftHand);
	// 스켈레탈메시 로드해서 할당
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_left.SKM_MannyXR_left'"));
	if (TempMesh.Succeeded())
	{
		LeftHandMesh->SetSkeletalMesh(TempMesh.Object);
		LeftHandMesh->SetRelativeLocation(FVector(-2.9f, -3.5f, 4.5f));
		LeftHandMesh->SetRelativeRotation(FRotator(-25, -180, 90));
	}

	// 오른손 메시
	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandMesh"));
	RightHandMesh->SetupAttachment(RightHand);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh2(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_right.SKM_MannyXR_right'"));
	if (TempMesh2.Succeeded())
	{
		RightHandMesh->SetSkeletalMesh(TempMesh2.Object);
		RightHandMesh->SetRelativeLocation(FVector(-2.9f, 3.5f, 4.5f));
		RightHandMesh->SetRelativeRotation(FRotator(25, 0, 90));
	}

	damageUI = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("damageUI"));
	damageUI->SetupAttachment(VRCamera);
	damageUI->SetRelativeLocationAndRotation(FVector(40, 0, 0), FRotator(90, 0, 0));
	damageUI->SetVisibility(false);

	backgroundColor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("backgroundColor"));
	backgroundColor->SetupAttachment(VRCamera);
	backgroundColor->SetRelativeLocationAndRotation(FVector(40, 0, 0), FRotator(90, 0, 0));
	backgroundColor->SetVisibility(false);

	victoryEndUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("victoryEndUI"));
	victoryEndUI->SetupAttachment(VRCamera);
	victoryEndUI->SetRelativeLocationAndRotation(FVector(35, 0, -5), FRotator(0, 180, 0));
	victoryEndUI->SetVisibility(false);

	defeatEndUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("defeatEndUI"));
	defeatEndUI->SetupAttachment(VRCamera);
	defeatEndUI->SetRelativeLocationAndRotation(FVector(35, 0, -5), FRotator(0, 180, 0));
	defeatEndUI->SetVisibility(false);

	waveStartUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("waveStartUI"));
	waveStartUI->SetupAttachment(VRCamera);
	waveStartUI->SetRelativeLocationAndRotation(FVector(35, 0, -5), FRotator(0, 180, 0));
	waveStartUI->SetVisibility(false);

	waveEndUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("waveEndUI"));
	waveEndUI->SetupAttachment(VRCamera);
	waveEndUI->SetRelativeLocationAndRotation(FVector(35, 0, -5), FRotator(0, 180, 0));
	waveEndUI->SetVisibility(false);

	GetCharacterMovement()->Crouch();

	HP = maxHP;

	VRCamera->bUsePawnControlRotation = false;
	

	mainWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("mainWidgetComp"));
	mainWidgetComp->SetupAttachment(VRCamera);
	mainWidgetComp->SetRelativeLocationAndRotation(FVector(50, 0, 0), FRotator(0, 180, 0));

	ConstructorHelpers::FClassFinder<UUserWidget> playerUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/WB_MainUI.WB_MainUI_C'"));
	if (playerUI.Succeeded())
	{
		mainWidgetComp->SetWidgetClass(playerUI.Class);
	}
	mainWidgetComp->InitWidget();
	
	// 집게손가락
	RightAim = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightAim"));
	RightAim->SetupAttachment(RootComponent);
	RightAim->SetTrackingMotionSource(FName("RightAim"));

	WidgetInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteractionComp"));
	WidgetInteractionComp->SetupAttachment(RightAim);
}

// Called when the game starts or when spawned
void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();

	mainUI = Cast<UPlayerMainWidget>(mainWidgetComp->GetUserWidgetObject());
	auto gm = Cast<AKillingFloorGameModeBase>(GetWorld()->GetAuthGameMode());
	gm->main_UI = mainUI;
	gm->player = this;

	// Enhanced Input 사용처리
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

	// 태어날 때 속력을 speedWalk 로 설정
	GetCharacterMovement()->MaxWalkSpeed = speedWalk;

	// 태어날 때 무기를 가지지 않은 상태로 설정
	currentWeaponType = EWeaponType::NONE;

	// HMD 가 연결되어 있지 않다면
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
	{
		// Hand 를 테스트할 수 있는 위치로 이동
		RightHand->SetRelativeLocation(FVector(20, 15, -5));
		LeftHand->SetRelativeLocation(FVector(20, -15, -5));
		// 카메라의 UsePawnControlRotation 을 활성화
		VRCamera->bUsePawnControlRotation = true;
		

		// player ui space 를 screen 으로 설정
		//mainWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	}
	// 만약 HMD 가 연결되어 있다면
	else
	{
		// 기본 tracking offset 설정
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);

		//mainWidgetComp->SetRelativeLocation(FVector(50, -5, 0));
		//damageUI->SetRelativeLocation(FVector(40, -5, 0));
		//backgroundColor->SetRelativeLocation(FVector(40, -5, 0));
		//victoryEndUI->SetRelativeLocation(FVector(35, -5, -5));
		//defeatEndUI->SetRelativeLocation(FVector(35, -5, -5));

		// player ui space 를 world 으로 설정
		//mainWidgetComp->SetWidgetSpace(EWidgetSpace::World);
	}
	mainWidgetComp->SetWidgetSpace(EWidgetSpace::World);
}

// Called every frame
void AVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	mainUI->PrintShopDirection();
	mainUI->PrintShopDistance();
}

// Called to bind functionality to input
void AVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto InputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (InputSystem)
	{
		// Binding
		InputSystem->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AVRPlayer::Move);
		InputSystem->BindAction(IA_Move, ETriggerEvent::Completed, this, &AVRPlayer::MoveEnd);
		InputSystem->BindAction(IA_Mouse, ETriggerEvent::Triggered, this, &AVRPlayer::Turn);

		InputSystem->BindAction(IA_Jump, ETriggerEvent::Started, this, &AVRPlayer::OnActionJump);

		InputSystem->BindAction(IA_Fire, ETriggerEvent::Started, this, &AVRPlayer::OnActionFirePressed);
		InputSystem->BindAction(IA_Fire, ETriggerEvent::Completed, this, &AVRPlayer::OnActionFireReleased);

		InputSystem->BindAction(IA_Run, ETriggerEvent::Started, this, &AVRPlayer::OnActionRunPressed);
		InputSystem->BindAction(IA_Run, ETriggerEvent::Completed, this, &AVRPlayer::OnActionRunReleased);

		InputSystem->BindAction(IA_Crouch, ETriggerEvent::Started, this, &AVRPlayer::OnActionCrouchPressed);
		InputSystem->BindAction(IA_Crouch, ETriggerEvent::Completed, this, &AVRPlayer::OnActionCrouchReleased);

		InputSystem->BindAction(IA_ChooseHandgun, ETriggerEvent::Started, this, &AVRPlayer::OnActionChooseHandgun);
		InputSystem->BindAction(IA_ChooseRifle, ETriggerEvent::Started, this, &AVRPlayer::OnActionChooseRifle);
		InputSystem->BindAction(IA_ChooseLauncher, ETriggerEvent::Started, this, &AVRPlayer::OnActionChooseLauncher);
		InputSystem->BindAction(IA_ChooseWeapon, ETriggerEvent::Started, this, &AVRPlayer::OnActionVRChooseWeapon);
	}
}

void AVRPlayer::Move(const FInputActionValue& Values)
{
	// 사용자의 입력에 따라 앞뒤좌우로 이동하고 싶다.
	FVector2D Axis = Values.Get<FVector2D>();
	// HMD 가 연결되어 있지 않다면
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
	{
		AddMovementInput(GetActorForwardVector(), Axis.X);
		AddMovementInput(GetActorRightVector(), Axis.Y);
	}
	// 만약 HMD 가 연결되어 있다면
	else
	{
		AddMovementInput(VRCamera->GetForwardVector(), Axis.X);
		AddMovementInput(VRCamera->GetRightVector(), Axis.Y);
	}

	moveSeconds += GetWorld()->DeltaTimeSeconds;
	if (moveSeconds >= 5.0f)
	{
		OnActionRunPressed(Values);
	}
}

void AVRPlayer::MoveEnd(const FInputActionValue& Values)
{
	moveSeconds = 0;
	OnActionRunReleased(Values);
}

void AVRPlayer::Turn(const FInputActionValue& Values)
{
	FVector2D Axis = Values.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AVRPlayer::OnActionJump(const FInputActionValue& Values)
{
	Jump();
}

void AVRPlayer::OnActionFirePressed(const FInputActionValue& Values)
{
	// UI 에 이벤트 전달하고 싶다.
	if (bisShopIn == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fire!!!!!"));
		if (WidgetInteractionComp)
		{
			WidgetInteractionComp->PressPointerKey(EKeys::LeftMouseButton);
		}
		return;
	}

	if (currentWeaponType != EWeaponType::NONE)
	{
		// 카메라를 흔들고싶다.
		APlayerCameraManager* cameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		// 만약 이미 흔들고 있었다면
		if (canShakeInstance != nullptr && canShakeInstance->IsFinished() == false)
		{
			// 취소하고
			cameraManager->StopCameraShake(canShakeInstance);
		}
		// 흔들고싶다.
		canShakeInstance = cameraManager->StartCameraShake(cameraShakeFactory);

		DoFire();
	}
}

void AVRPlayer::OnActionFireReleased(const FInputActionValue& Values)
{
	// UI 에 이벤트 전달하고 싶다.
	if (bisShopIn == true)
	{
		if (WidgetInteractionComp)
		{
			WidgetInteractionComp->ReleasePointerKey(EKeys::LeftMouseButton);
		}
		return;
	}

	// 총을 들고 있지 않았다면
	if (currentWeaponType == EWeaponType::NONE)
	{
		return;
	}
	// 만약 웨폰타입이 라이플이면
	if (currentWeaponType == EWeaponType::RIFLE)
	{
		// 타이머를 멈춘다.
		rifleWeapon->GetWorldTimerManager().ClearTimer(rifleWeapon->FireTimer);

	}
}

void AVRPlayer::DoFire()
{
	
	//UE_LOG(LogTemp, Warning, TEXT("Fire!!!!!"));
	if (currentWeaponType == EWeaponType::HANDGUN)
	{
		handgunWeapon->FirePistol();
	}
	else if (currentWeaponType == EWeaponType::RIFLE)
	{
		rifleWeapon->FireRifle();
	}
	else if (currentWeaponType == EWeaponType::LAUNCHER)
	{
		launcherWeapon->FireLauncher();
	}
	
	
	// 총기 사운드 출력
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());

}

void AVRPlayer::OnActionRunPressed(const FInputActionValue& Values)
{
	GetCharacterMovement()->MaxWalkSpeed = speedRun;
}

void AVRPlayer::OnActionRunReleased(const FInputActionValue& Values)
{
	GetCharacterMovement()->MaxWalkSpeed = speedWalk;
}

void AVRPlayer::OnActionCrouchPressed(const FInputActionValue& Values)
{
	GetCharacterMovement()->MaxWalkSpeedCrouched = speedCrouch;
	Crouch();
}

void AVRPlayer::OnActionCrouchReleased(const FInputActionValue& Values)
{
	GetCharacterMovement()->MaxWalkSpeedCrouched = speedWalk;
	UnCrouch();
}

void AVRPlayer::OnActionChooseHandgun(const FInputActionValue& Values)
{
	if (handgunWeapon)
	{
		ChooseWeapon(EWeaponType::HANDGUN);
	}
}

void AVRPlayer::OnActionChooseRifle(const FInputActionValue& Values)
{
	if (rifleWeapon)
	{
		ChooseWeapon(EWeaponType::RIFLE);
	}
}

void AVRPlayer::OnActionChooseLauncher(const FInputActionValue& Values)
{
	if (launcherWeapon)
	{
		ChooseWeapon(EWeaponType::LAUNCHER);
	}
}

void AVRPlayer::OnActionVRChooseWeapon(const FInputActionValue& Values)
{
	if (currentWeaponType == EWeaponType::NONE)
	{
		if (handgunWeapon)
		{
			ChooseWeapon(EWeaponType::HANDGUN);
		}
		else if (rifleWeapon)
		{
			ChooseWeapon(EWeaponType::RIFLE);
		}
		else if (launcherWeapon)
		{
			ChooseWeapon(EWeaponType::LAUNCHER);
		}
	}
	else if (currentWeaponType == EWeaponType::HANDGUN)
	{
		if (rifleWeapon)
		{
			ChooseWeapon(EWeaponType::RIFLE);
		}
		else if (launcherWeapon)
		{
			ChooseWeapon(EWeaponType::LAUNCHER);
		}
	}
	else if (currentWeaponType == EWeaponType::RIFLE)
	{
		if (launcherWeapon)
		{
			ChooseWeapon(EWeaponType::LAUNCHER);
		}
		else if (handgunWeapon)
		{
			ChooseWeapon(EWeaponType::HANDGUN);
		}
	}
	else if (currentWeaponType == EWeaponType::LAUNCHER)
	{
		if (handgunWeapon)
		{
			ChooseWeapon(EWeaponType::HANDGUN);
		}
		else if (rifleWeapon)
		{
			ChooseWeapon(EWeaponType::RIFLE);
		}
	}
}

void AVRPlayer::ChooseWeapon(EWeaponType weaponType)
{
	currentWeaponType = weaponType;
	if (weaponType == EWeaponType::HANDGUN)
	{
		handgunWeapon->HandgunMesh->SetVisibility(true);
		if (rifleWeapon)
		{
			rifleWeapon->RifleMesh->SetVisibility(false);
		}
		if (launcherWeapon)
		{
			launcherWeapon->LauncherMesh->SetVisibility(false);
		}
	}
	else if (weaponType == EWeaponType::RIFLE)
	{
		if (handgunWeapon)
		{
			handgunWeapon->HandgunMesh->SetVisibility(false);
		}
		rifleWeapon->RifleMesh->SetVisibility(true);
		if (launcherWeapon)
		{
			launcherWeapon->LauncherMesh->SetVisibility(false);
		}
	}
	else if (weaponType == EWeaponType::LAUNCHER)
	{
		if (handgunWeapon)
		{
			handgunWeapon->HandgunMesh->SetVisibility(false);
		}
		if (rifleWeapon)
		{
			rifleWeapon->RifleMesh->SetVisibility(false);
		}
		launcherWeapon->LauncherMesh->SetVisibility(true);
	}
}

