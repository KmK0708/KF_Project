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

	// ������� �Է¿� ���� �յ��¿�� �̵��ϰ� �ʹ�.
	// �ʿ�Ӽ� : �̵��ӵ�, �Է¾׼�, �Է¸������ؽ�Ʈ
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


	// �̵�ó�� �Լ�
	void Move(const FInputActionValue& Values);
	void MoveEnd(const FInputActionValue& Values);
	// ȸ��ó�� �Լ�
	void Turn(const FInputActionValue& Values);
	// ����ó�� �Լ�
	void OnActionJump(const FInputActionValue& Values);
	
	// �� ��� ��ư ������ �� ó���� �Լ�
	void OnActionFirePressed(const FInputActionValue& Values);
	// �� ��� ��ư ���� �� ó���� �Լ�
	void OnActionFireReleased(const FInputActionValue& Values);
	void DoFire();

	// �޸��� ó�� �Լ�
	void OnActionRunPressed(const FInputActionValue& Values);
	void OnActionRunReleased(const FInputActionValue& Values);

	// Crouch ó�� �Լ�
	void OnActionCrouchPressed(const FInputActionValue& Values);
	void OnActionCrouchReleased(const FInputActionValue& Values);

	// ChooseWeapon ó�� �Լ�
	void OnActionChooseHandgun(const FInputActionValue& Values);
	void OnActionChooseRifle(const FInputActionValue& Values);
	void OnActionChooseLauncher(const FInputActionValue& Values);
	// VR ChooseWeapon ó�� �Լ�
	void OnActionVRChooseWeapon(const FInputActionValue& Values);

public:
	
	// ��Ʈ�ѷ�
	UPROPERTY(VisibleAnywhere, Category = "MotionController")
	class UMotionControllerComponent* LeftHand;
	UPROPERTY(VisibleAnywhere, Category = "MotionController")
	class UMotionControllerComponent* RightHand;

	// ����� �� ��
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
	// �ѱ� ����
	UPROPERTY(EditAnywhere, Category = "Sound")
	class USoundBase* FireSound;
	// �÷��̾ ���� �ֿ��� �� ����
	UPROPERTY(EditAnywhere, Category = "Weapon")
	class AHandgun* handgunWeapon;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	class ARifle* rifleWeapon;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	class ALauncher* launcherWeapon;

	// �÷��̾ �� ������ �� �ִ� ����
	UPROPERTY(EditAnywhere, Category = "Weapon")
	EWeaponType currentWeaponType;


public:
	// CameraShake
	UPROPERTY(EditAnywhere, Category = "VRCamera")
	TSubclassOf<class UCameraShakeBase> cameraShakeFactory;
	UPROPERTY()
	class UCameraShakeBase* canShakeInstance;
	// ī�޶�
	UPROPERTY(VisibleAnywhere, Category = "VRCamera")
	class UCameraComponent* VRCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UWidgetComponent* mainWidgetComp;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UPlayerMainWidget* mainUI;

	void ChooseWeapon(EWeaponType weaponType);

	// ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
	int32 HP;
	UPROPERTY(EditAnywhere, Category = "HP")
	int32 maxHP = 100;

	// ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Money")
	int32 money = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 gunCount = 0;

	float moveSeconds = 0;

	// ���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	bool bisShopIn = false;

	// ���Լհ��� ǥ���� �����Ʈ�ѷ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HandComp")
	class UMotionControllerComponent* RightAim;

protected: // Widget ���� �Ӽ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess = true))
	class UWidgetInteractionComponent* WidgetInteractionComp;
};

