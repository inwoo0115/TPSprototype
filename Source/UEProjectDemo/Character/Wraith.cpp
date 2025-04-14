// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Wraith.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// 캐릭터 기본 설정
AWraith::AWraith()
{
	//컨트롤러의 회전을 받아서 설정하는 모드
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//무브먼트 설정
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;

	//캡슐 컴포넌트 설정
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);

	//Mesh 위치 설정
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	//리소스 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(TEXT("/ Game / ParagonWraith / Characters / Heroes / Wraith / Meshes / Wraith.Wraith"));
	if (CharacterMesh.Object)
	{
		// Mesh컴포넌트에 접근
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
