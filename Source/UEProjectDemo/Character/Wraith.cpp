// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Wraith.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/TimelineComponent.h"
#include "EnhancedInputComponent.h"

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

	//케이블 설정
	CableComponent = nullptr;
}

void AWraith::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// Binding
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AWraith::Jump);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWraith::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWraith::Look);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AWraith::Run);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AWraith::Attack);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AWraith::AimIn);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AWraith::AimOut);
	EnhancedInputComponent->BindAction(RopeAction, ETriggerEvent::Triggered, this, &AWraith::Rope);
	EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AWraith::Throw);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AWraith::Reload);
}

void AWraith::BeginPlay()
{
	Super::BeginPlay();

	// Timeline Binding
	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("AimUpdate"));

	AimTimeline.AddInterpFloat(AimTimelineFloatCurve, ProgressUpdate);
}

void AWraith::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 타임라인 tick 갱신
	AimTimeline.TickTimeline(DeltaTime);

	// 줄에 매달려 있을 경우 힘 적용
	ImpulseTension();
}

void AWraith::Move(const FInputActionValue& Value)
{
	FVector2D Movement = Value.Get<FVector2D>();

	// 컨트롤러 회전 값
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
	
	// 방향 구하기.
	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 무브먼트 컴포넌트에 값 전달.
	AddMovementInput(ForwardVector, Movement.X);
	AddMovementInput(RightVector, Movement.Y);
}

void AWraith::Look(const FInputActionValue& Value)
{
	FVector2D Movement = Value.Get<FVector2D>();
	
	// 시점 회전
	AddControllerPitchInput(Movement.Y * -1);
	AddControllerYawInput(Movement.X);
}

void AWraith::Jump(const FInputActionValue& Value)
{
	if (IsJump)
	{
		Super::Jump();
	}
}

void AWraith::Run(const FInputActionValue& Value)
{
	if (IsRun)
	{
		IsRun = false;
		GetCharacterMovement()->MaxWalkSpeed += 300.0;
	}
	else
	{
		IsRun = true;
		GetCharacterMovement()->MaxWalkSpeed -= 300.0;
	}
}

void AWraith::Attack(const FInputActionValue& Value)
{
	
}

void AWraith::AimIn(const FInputActionValue& Value)
{
	AimTimeline.Play();
}

void AWraith::AimOut(const FInputActionValue& Value)
{
	AimTimeline.Reverse();
}

void AWraith::AimUpdate(float Alpha)
{
	float Aim = FMath::Lerp(300, 40, Alpha);
	SpringArm->TargetArmLength = Aim;
}

void AWraith::Rope(const FInputActionValue& Value)
{
	if (IsGrappling)
	{
		IsGrappling = false;
		// 케이블 안보이게 수정
		if (CableComponent)
		{
			CableComponent->DestroyComponent();
			CableComponent = nullptr;
		}
	}
	else
	{
		FVector3d StartLocation = GetActorLocation();
		FVector3d DirectionVector = GetActorForwardVector();
		FHitResult HitResult;

		bool bHit = GetWorld()->SweepSingleByProfile(
			HitResult,
			StartLocation + DirectionVector * 50,
			StartLocation + DirectionVector * 2000,
			FQuat::Identity,
			TEXT("InteractionTrace"),
			FCollisionShape::MakeSphere(200.0f)
		);

		if (bHit)
		{
			//TODO : Rope object로 캐스트 
			//HitResult.GetActor()
		}

		// 물체와 플레이어 잇는 케이블 설치
		CableComponent = NewObject<UCableComponent>(this, UCableComponent::StaticClass());
		CableComponent->RegisterComponent();
		//CableComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		CableComponent->SetVisibility(true);

	}
}


// 장력 탄성력 적용
void AWraith::ImpulseTension()
{
	if (IsGrappling)
	{
		// 장력 + 탄성력
		FVector3d AddForce;

		//매달려 있는 물체와 액터 간의 방향벡터
		FVector3d RopeDirectionVector = RopeLocation - GetActorLocation();
		double RopeLength = RopeDirectionVector.Length();

		//특정 거리 이상일 때만 적용
		if (RopeLength > 200.0)
		{
			RopeDirectionVector.Normalize();
			//현재 가속도 벡터
			FVector3d CurrentAcceleration = GetCharacterMovement()->GetCurrentAcceleration();
			//중력 벡터
			FVector3d GravityVector = FVector3d(0.0f, 0.0f, GetCharacterMovement()->GetGravityZ());

			//내적
			double DotAcceleration = RopeDirectionVector | CurrentAcceleration;
			double DotGravity = RopeDirectionVector | GravityVector;


			double TotalForceScalar = 0.0;
			double ElasticForceScalar = 0.0;
			// 내적 값이 양수 일 경우에 더하기
			if (DotAcceleration > 0.0)
			{
				TotalForceScalar += DotAcceleration;
			}
			if (DotGravity > 0.0)
			{
				TotalForceScalar += DotGravity;
			}

			// 추가 가속도 보정
			TotalForceScalar *= -1.3;

			// 특정 거리 이상일 경우 탄성력 적용
			if (RopeLength > 500.0)
			{
				ElasticForceScalar = RopeLength * 800.0;
			}

			// 장력 + 탄성력 적용
			AddForce = (TotalForceScalar + ElasticForceScalar) * RopeDirectionVector;
			GetCharacterMovement()->AddForce(AddForce);
		}
	}
}


void AWraith::Throw(const FInputActionValue& Value)
{
	// Anim Montage 실행
	if (IsThrow)
	{
		static ConstructorHelpers::FObjectFinder<UAnimMontage> ThrowMontage(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Ability_E_Montage.Ability_E_Montage"));
		if (ThrowMontage.Object)
		{
			PlayAnimMontage(ThrowMontage.Object, 1.0f);
		}

		//2초 쿨타임 적용
		IsThrow = false;
		GetWorld()->GetTimerManager().SetTimer(
			DelayTimeHandle,
			[this]()
			{
				IsThrow = true;
			},
			2.0f,
			false
		);
		
	}
	// TODO : 수류탄 액터 스폰 추가
}

void AWraith::Reload(const FInputActionValue& Value)
{
}

void AWraith::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	IsJump = false;

	// 바닥 착지 후 0.5초 후 점프 가능
	GetWorld()->GetTimerManager().SetTimer(
		DelayTimeHandle,
		[this]()
		{
			IsJump = true;
		},
		0.5f,
		false
		);
}
