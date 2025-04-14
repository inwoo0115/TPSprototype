// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Wraith.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// ĳ���� �⺻ ����
AWraith::AWraith()
{
	//��Ʈ�ѷ��� ȸ���� �޾Ƽ� �����ϴ� ���
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//�����Ʈ ����
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;

	//ĸ�� ������Ʈ ����
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);

	//Mesh ��ġ ����
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	//���ҽ� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(TEXT("/ Game / ParagonWraith / Characters / Heroes / Wraith / Meshes / Wraith.Wraith"));
	if (CharacterMesh.Object)
	{
		// Mesh������Ʈ�� ����
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(TEXT("/Game/Demo/Blueprints/Wraith/ABP_Wraith.ABP_Wraith_C"));
	if (CharacterAnim.Class)
	{
		GetMesh()->SetAnimClass(CharacterAnim.Class);
	}
}

void AWraith::BeginPlay()
{
}

void AWraith::Tick(float DeltaTime)
{
}

void AWraith::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
}

void AWraith::Move(const FInputActionValue& Value)
{
}

void AWraith::Look(const FInputActionValue& Value)
{
}

void AWraith::Zoom(const FInputActionValue& Value)
{
}

void AWraith::Jump(const FInputActionValue& Value)
{
}

void AWraith::Run(const FInputActionValue& Value)
{
}

void AWraith::Attack(const FInputActionValue& Value)
{
}

void AWraith::Aim(const FInputActionValue& Value)
{
}

void AWraith::Rope(const FInputActionValue& Value)
{
}

void AWraith::Throw(const FInputActionValue& Value)
{
}
