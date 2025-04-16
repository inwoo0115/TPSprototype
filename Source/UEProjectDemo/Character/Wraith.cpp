// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Wraith.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/TimelineComponent.h"
#include "EnhancedInputComponent.h"

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

	//���̺� ����
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

	// Ÿ�Ӷ��� tick ����
	AimTimeline.TickTimeline(DeltaTime);

	// �ٿ� �Ŵ޷� ���� ��� �� ����
	ImpulseTension();
}

void AWraith::Move(const FInputActionValue& Value)
{
	FVector2D Movement = Value.Get<FVector2D>();

	// ��Ʈ�ѷ� ȸ�� ��
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
	
	// ���� ���ϱ�.
	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// �����Ʈ ������Ʈ�� �� ����.
	AddMovementInput(ForwardVector, Movement.X);
	AddMovementInput(RightVector, Movement.Y);
}

void AWraith::Look(const FInputActionValue& Value)
{
	FVector2D Movement = Value.Get<FVector2D>();
	
	// ���� ȸ��
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
		// ���̺� �Ⱥ��̰� ����
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
			//TODO : Rope object�� ĳ��Ʈ 
			//HitResult.GetActor()
		}

		// ��ü�� �÷��̾� �մ� ���̺� ��ġ
		CableComponent = NewObject<UCableComponent>(this, UCableComponent::StaticClass());
		CableComponent->RegisterComponent();
		//CableComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		CableComponent->SetVisibility(true);

	}
}


// ��� ź���� ����
void AWraith::ImpulseTension()
{
	if (IsGrappling)
	{
		// ��� + ź����
		FVector3d AddForce;

		//�Ŵ޷� �ִ� ��ü�� ���� ���� ���⺤��
		FVector3d RopeDirectionVector = RopeLocation - GetActorLocation();
		double RopeLength = RopeDirectionVector.Length();

		//Ư�� �Ÿ� �̻��� ���� ����
		if (RopeLength > 200.0)
		{
			RopeDirectionVector.Normalize();
			//���� ���ӵ� ����
			FVector3d CurrentAcceleration = GetCharacterMovement()->GetCurrentAcceleration();
			//�߷� ����
			FVector3d GravityVector = FVector3d(0.0f, 0.0f, GetCharacterMovement()->GetGravityZ());

			//����
			double DotAcceleration = RopeDirectionVector | CurrentAcceleration;
			double DotGravity = RopeDirectionVector | GravityVector;


			double TotalForceScalar = 0.0;
			double ElasticForceScalar = 0.0;
			// ���� ���� ��� �� ��쿡 ���ϱ�
			if (DotAcceleration > 0.0)
			{
				TotalForceScalar += DotAcceleration;
			}
			if (DotGravity > 0.0)
			{
				TotalForceScalar += DotGravity;
			}

			// �߰� ���ӵ� ����
			TotalForceScalar *= -1.3;

			// Ư�� �Ÿ� �̻��� ��� ź���� ����
			if (RopeLength > 500.0)
			{
				ElasticForceScalar = RopeLength * 800.0;
			}

			// ��� + ź���� ����
			AddForce = (TotalForceScalar + ElasticForceScalar) * RopeDirectionVector;
			GetCharacterMovement()->AddForce(AddForce);
		}
	}
}


void AWraith::Throw(const FInputActionValue& Value)
{
	// Anim Montage ����
	if (IsThrow)
	{
		static ConstructorHelpers::FObjectFinder<UAnimMontage> ThrowMontage(TEXT("/Game/ParagonWraith/Characters/Heroes/Wraith/Animations/Ability_E_Montage.Ability_E_Montage"));
		if (ThrowMontage.Object)
		{
			PlayAnimMontage(ThrowMontage.Object, 1.0f);
		}

		//2�� ��Ÿ�� ����
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
	// TODO : ����ź ���� ���� �߰�
}

void AWraith::Reload(const FInputActionValue& Value)
{
}

void AWraith::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	IsJump = false;

	// �ٴ� ���� �� 0.5�� �� ���� ����
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
