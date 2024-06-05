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
#include "KismetTraceUtils.h"
#include "Blueprint/UserWidget.h"
#include "Character/RubeeWidget.h"
#include "Engine/HitResult.h"
#include "Engine/LocalPlayer.h"
#include "Equipment/Item.h"
#include "Equipment/WeaponComponent.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/HUD.h"

DEFINE_LOG_CATEGORY(LogRubee);

ARubee::ARubee()
	: pickUpRange(250.0f)
	, bIsDebug(false)
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	cameraFP = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFirstPerson"));
	cameraFP->SetupAttachment(GetCapsuleComponent());
	cameraFP->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	cameraFP->bUsePawnControlRotation = true;

	USkeletalMeshComponent& mesh = *GetMesh();
	mesh.SetOnlyOwnerSee(true);
	mesh.SetupAttachment(cameraFP);
	mesh.bCastDynamicShadow = false;
	mesh.CastShadow			= false;
	mesh.SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void ARubee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult hitInfo;
	FVector origin = GetActorLocation() + cameraFP->GetRelativeLocation();
	FVector destination = origin + cameraFP->GetForwardVector() * pickUpRange;

	bool bIsHit = GetWorld()->LineTraceSingleByChannel(hitInfo, origin, destination, ECC_Visibility);
	if (bIsDebug)
	{
		DrawDebugLineTraceSingle(GetWorld(), 
								 origin, 
								 destination, 
								 EDrawDebugTrace::Type::ForDuration, 
								 bIsHit, hitInfo,
								 FLinearColor::Red, 
								 FLinearColor::Blue,
								 10.0f);
	}

	if (bIsHit)
	{
		focusedItem = Cast<AItem>(hitInfo.GetActor());
		if (focusedItem && rubeeWidget)
		{
			rubeeWidget->set_interaction_text("Take [E]");
		}
	} else {
		focusedItem = nullptr;
		if (rubeeWidget)
		{
			rubeeWidget->set_interaction_text("");
		}
	}
	
}

UCameraComponent* ARubee::get_camera_fp() const
{
	return cameraFP;
}

// Called when the game starts or when spawned
void ARubee::BeginPlay()
{
	Super::BeginPlay();

	experience = FindComponentByClass<UExperienceComponent>();
	health	   = FindComponentByClass<UHealthComponent>();
	honey	   = FindComponentByClass<UHoneyComponent>();

	if (rubeeWidget)
	{
		CreateWidget(GetWorld(), rubeeWidget.GetClass(), FName("RubeeWidget"));
		rubeeWidget->AddToViewport();
		rubeeWidget->set_interaction_text("");
	}
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

void ARubee::interact()
{
	if (!focusedItem)
	{
		return;
	}

	focusedItem->pick(this);
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

		EnhancedInputComponent->BindAction(interactAction, ETriggerEvent::Triggered, this, &ARubee::interact);
	} else {
		UE_LOG(LogRubee, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

