// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Rubee.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/ExperienceComponent.h"
#include "Character/HealthComponent.h"
#include "Character/HoneyComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogRubee);

ARubee::ARubee()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	cameraFP = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	cameraFP->SetupAttachment(GetCapsuleComponent());
	cameraFP->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	cameraFP->bUsePawnControlRotation = true;

	skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	skeletalMesh->SetOnlyOwnerSee(true);
	skeletalMesh->SetupAttachment(cameraFP);
	skeletalMesh->bCastDynamicShadow = false;
	skeletalMesh->CastShadow		 = false;

	skeletalMesh->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	experience = FindComponentByClass<UExperienceComponent>();
	health	   = FindComponentByClass<UHealthComponent>();
	honey	   = FindComponentByClass<UHoneyComponent>();
}

USkeletalMeshComponent* ARubee::get_skeletal_mesh() const
{
	return skeletalMesh;
}

UCameraComponent* ARubee::get_camera_fp() const
{
	return cameraFP;
}

// Called when the game starts or when spawned
void ARubee::BeginPlay()
{
	Super::BeginPlay();
}

void ARubee::move(const FInputActionValue& value)
{
	// input is a Vector2D
	FVector2D movementVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), movementVector.Y);
		AddMovementInput(GetActorRightVector(), movementVector.X);
	}
}

void ARubee::look(const FInputActionValue& value)
{
	// input is a Vector2D
	FVector2D lookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(lookAxisVector.X);
		AddControllerPitchInput(lookAxisVector.Y);
	}
}

// Called to bind functionality to input
void ARubee::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent != nullptr)
	{
		// Jumping
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ARubee::move);

		// Looking
		EnhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &ARubee::look);
	} else {
		UE_LOG(LogRubee, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

