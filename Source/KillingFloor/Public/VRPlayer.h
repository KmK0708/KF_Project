// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "VRPlayer.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NONE,
	HANDGUN,
	RIFLE,
	LAUNCHER,
};

UCLASS()
class KILLINGFLOOR_API AVRPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 사용자의 입력에 따라 앞뒤좌우로 이동하고 싶다.
	// 필요속성 : 이동속도, 입력액션, 입력매핑컨텍스트
	float speedRun = 600;
	float speedWalk = 400;
	float speedCrouch = 200;

	// Input Mapping Context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_VRInput;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Hand;
	// Input Action for Move
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Move;
	// Input Action for Turn
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Mouse;
	// Input Action for Fire
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Fire;
	// Input Action for Jump
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Jump;
	// Input Action for Run
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Run;
	// Input Action for Crouch
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Crouch;
	// Input Action for ChooseWeapon
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_ChooseHandgun;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_ChooseRifle;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_ChooseLauncher;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_ChooseWeapon;


	// 이동처리 함수
	void Move(const FInputActionValue& Values);
	void MoveEnd(const FInputActionValue& Values);
	// 회전처리 함수
	void Turn(const FInputActionValue& Values);
	// 점프처리 함수
	void OnActionJump(const FInputActionValue& Values);
	
	// 총 쏘기 버튼 눌렀을 때 처리할 함수
	void OnActionFirePressed(const FInputActionValue& Values);
	// 총 쏘기 버튼 뗐을 때 처리할 함수
	void OnActionFireReleased(const FInputActionValue& Values);
	void DoFire();

	// 달리기 처리 함수
	void OnActionRunPressed(const FInputActionValue& Values);
	void OnActionRunReleased(const FInputActionValue& Values);

	// Crouch 처리 함수
	void OnActionCrouchPressed(const FInputActionValue& Values);
	void OnActionCrouchReleased(const FInputActionValue& Values);

	// ChooseWeapon 처리 함수
	void OnActionChooseHandgun(const FInputActionValue& Values);
	void OnActionChooseRifle(const FInputActionValue& Values);
	void OnActionChooseLauncher(const FInputActionValue& Values);
	// VR ChooseWeapon 처리 함수
	void OnActionVRChooseWeapon(const FInputActionValue& Values);

public:
	
	// 컨트롤러
	UPROPERTY(VisibleAnywhere, Category = "MotionController")
	class UMotionControllerComponent* LeftHand;
	UPROPERTY(VisibleAnywhere, Category = "MotionController")
	class UMotionControllerComponent* RightHand;

	// 사용할 손 모델
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MotionController")
	class USkeletalMeshComponent* LeftHandMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MotionController")
	class USkeletalMeshComponent* RightHandMesh;

	// subUI
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UStaticMeshComponent* damageUI;
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UStaticMeshComponent* backgroundColor;
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* victoryEndUI;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UWidgetComponent* defeatEndUI;
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* waveStartUI;
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* waveEndUI;

public:
	// 총기 사운드
	UPROPERTY(EditAnywhere, Category = "Sound")
	class USoundBase* FireSound;
	// 플레이어가 총을 주웠을 때 변수
	UPROPERTY(EditAnywhere, Category = "Weapon")
	class AHandgun* handgunWeapon;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	class ARifle* rifleWeapon;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	class ALauncher* launcherWeapon;

	// 플레이어가 총 구별할 수 있는 변수
	UPROPERTY(EditAnywhere, Category = "Weapon")
	EWeaponType currentWeaponType;


public:
	// CameraShake
	UPROPERTY(EditAnywhere, Category = "VRCamera")
	TSubclassOf<class UCameraShakeBase> cameraShakeFactory;
	UPROPERTY()
	class UCameraShakeBase* canShakeInstance;
	// 카메라
	UPROPERTY(VisibleAnywhere, Category = "VRCamera")
	class UCameraComponent* VRCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UWidgetComponent* mainWidgetComp;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UPlayerMainWidget* mainUI;

	void ChooseWeapon(EWeaponType weaponType);

	// 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
	int32 HP;
	UPROPERTY(EditAnywhere, Category = "HP")
	int32 maxHP = 100;

	// 돈
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Money")
	int32 money = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 gunCount = 0;

	float moveSeconds = 0;

	// 상점 들어갈때 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	bool bisShopIn = false;

	// 집게손가락 표시할 모션컨트롤러
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HandComp")
	class UMotionControllerComponent* RightAim;

protected: // Widget 관련 속성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess = true))
	class UWidgetInteractionComponent* WidgetInteractionComp;
};

