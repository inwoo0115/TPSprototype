// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TPSCharacterBase.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "Wraith.generated.h"

/**
 * 
 */
UCLASS()
class UEPROJECTDEMO_API AWraith : public ATPSCharacterBase
{
	GENERATED_BODY()
	
public: 
	AWraith();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	
	// 입력 액션

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Jump(const FInputActionValue& Value);

	void Run(const FInputActionValue& Value);

	void Attack(const FInputActionValue& Value);

	void AimIn(const FInputActionValue& Value);

	void AimOut(const FInputActionValue& Value);

	void Rope(const FInputActionValue& Value);

	void Throw(const FInputActionValue& Value);

	void Reload(const FInputActionValue& Value);

	// 이벤트 액션
	virtual void Landed(const FHitResult& Hit) override;

	// 입력 매핑

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RunAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RopeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ThrowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReloadAction;

	// 컴포넌트

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArm;

	// 에임 타임라인

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* AimTimelineFloatCurve;

	FTimeline AimTimeline;

	UFUNCTION()
	void AimUpdate(float Alpha);

	// 로프 액션
	
	FVector3d RopeLocation;

	UPROPERTY(EditAnywhere, Category = "Cable")
	UCableComponent* CableComponent;

	void ImpulseTension();

	// 플래그

	bool IsRun = false;

	bool IsAttack = false;

	bool IsJump = true;

	bool IsGrappling = false;

	bool IsThrow = true;

	// 타이어
	FTimerHandle DelayTimeHandle;
};
