// Copyright Epic Games, Inc. All Rights Reserved.

#include "Riot_ProjectCharacter.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Riot_Project.h"
#include "UObject/UnrealType.h"

ARiot_ProjectCharacter::ARiot_ProjectCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ARiot_ProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	// W should enter the Walk state by default. Running is enabled only while Run is held.
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	UpdateAnimationLocomotionState();
}

void ARiot_ProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateAnimationLocomotionState();
}

void ARiot_ProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARiot_ProjectCharacter::Move);
		if (RunAction)
		{
			EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &ARiot_ProjectCharacter::DoRunStart);
			EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ARiot_ProjectCharacter::DoRunEnd);
			EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Canceled, this, &ARiot_ProjectCharacter::DoRunEnd);
		}
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ARiot_ProjectCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARiot_ProjectCharacter::Look);
	}
	else
	{
		UE_LOG(LogRiot_Project, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ARiot_ProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ARiot_ProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ARiot_ProjectCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}

	// Also update immediately on input; Tick keeps the value accurate while accelerating or braking.
	UpdateAnimationLocomotionState();
}

void ARiot_ProjectCharacter::UpdateAnimationLocomotionState()
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		return;
	}

	FProperty* SpeedProperty = AnimInstance->GetClass()->FindPropertyByName(TEXT("Speed"));
	if (FFloatProperty* FloatSpeedProperty = CastField<FFloatProperty>(SpeedProperty))
	{
		FloatSpeedProperty->SetPropertyValue_InContainer(AnimInstance, GetVelocity().Size2D());
	}
	else if (FDoubleProperty* DoubleSpeedProperty = CastField<FDoubleProperty>(SpeedProperty))
	{
		DoubleSpeedProperty->SetPropertyValue_InContainer(AnimInstance, GetVelocity().Size2D());
	}

	const bool bFalling = GetCharacterMovement() && GetCharacterMovement()->IsFalling();
	for (TFieldIterator<FBoolProperty> PropertyIt(AnimInstance->GetClass()); PropertyIt; ++PropertyIt)
	{
		FBoolProperty* BoolProperty = *PropertyIt;
		FString ComparableName = BoolProperty->GetName();
		ComparableName.ReplaceInline(TEXT(" "), TEXT(""));
		ComparableName.RemoveFromStart(TEXT("b"), ESearchCase::CaseSensitive);
		if (ComparableName.Equals(TEXT("IsFalling"), ESearchCase::IgnoreCase))
		{
			BoolProperty->SetPropertyValue_InContainer(AnimInstance, bFalling);
			break;
		}
	}
}

void ARiot_ProjectCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ARiot_ProjectCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ARiot_ProjectCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ARiot_ProjectCharacter::DoRunStart()
{
	bIsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ARiot_ProjectCharacter::DoRunEnd()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
