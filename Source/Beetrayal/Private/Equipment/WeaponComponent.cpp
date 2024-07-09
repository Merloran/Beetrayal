// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/WeaponComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "KismetTraceUtils.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


UWeaponComponent::UWeaponComponent()
	: bIsProjectileCollide(false)
	, length(200.0)
	, attackGrid(1)
	, degrees(0.0)
	, randomHorizontalOffsetBounds(0.0)
	, randomVerticalOffsetBounds(0.0)
	, originOffset(100.0, 0.0, 10.0)
	, bIsDebug(false)
	, bIsAxisInverted(false)
	, bIsWingDistributionAttack(false)
	, attackCooldownMs(100.0f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UWeaponComponent::attach(ACharacter *target)
{
	character = target;
	if (!character || character->GetInstanceComponents().FindItemByClass<UWeaponComponent>())
	{
		return false;
	}

	FAttachmentTransformRules attachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(character->GetMesh(), attachmentRules, FName(TEXT("GripPoint")));
	
	character->AddInstanceComponent(this);

	APlayerController *playerController = Cast<APlayerController>(character->GetController());
	if (playerController)
	{
		UEnhancedInputLocalPlayerSubsystem *subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());
		if (subsystem)
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			subsystem->AddMappingContext(weaponMapping, 1);
		}

		UEnhancedInputComponent *enhancedInputComponent = Cast<UEnhancedInputComponent>(playerController->InputComponent);
		if (enhancedInputComponent)
		{
			enhancedInputComponent->BindAction(attackAction, ETriggerEvent::Triggered, this, &UWeaponComponent::attack);
		}
	}

	return true;
}

void UWeaponComponent::attack()
{
	UWorld *const world = GetWorld();
	if (!world || !character || attackGrid.X <= 0 || attackGrid.Y <= 0)
	{
		return;
	}

	APlayerController *playerController = Cast<APlayerController>(character->GetController());
	UCameraComponent *camera = character->GetComponentByClass<UCameraComponent>();
	FTimerManager &timerManager = world->GetTimerManager();
	if (timerManager.IsTimerActive(timer) || !playerController || !camera)
	{
		return;
	}
	
	onAttack.Broadcast(character);

	timerManager.SetTimer(timer, this, &UWeaponComponent::reset_cooldown, attackCooldownMs * 0.001f);

	const FRotator rotation = camera->GetComponentRotation();
	const FVector origin = GetOwner()->GetActorLocation() + rotation.RotateVector(originOffset);
	FVector2D deltaAngle = { degrees.X / attackGrid.X, degrees.Y / attackGrid.Y };

	FInt32Vector2 yBounds;
	yBounds.X = -attackGrid.Y / 2.0;
	yBounds.Y = attackGrid.Y / 2.0;
	FInt32Vector2 xBounds;
	xBounds.X = -attackGrid.X / 2.0;
	xBounds.Y = attackGrid.X / 2.0;
	const bool isYEven = ~attackGrid.Y & 1;
	const bool isXEven = ~attackGrid.X & 1;

	for (int32 y = yBounds.X; y <= yBounds.Y; ++y)
	{
		if (isYEven && y == 0)
		{
			continue;
		}
		for (int32 x = xBounds.X; x <= xBounds.Y; ++x)
		{
			if (isXEven && x == 0)
			{
				continue;
			}
			FVector pointOnSphere = to_world_coordinates({ length, deltaAngle.X * x, deltaAngle.Y * y });
			FVector destination = origin
								+ rotation.RotateVector(pointOnSphere)
								+ camera->GetRightVector() * FMath::FRandRange(randomHorizontalOffsetBounds.X, randomHorizontalOffsetBounds.Y)
								+ camera->GetUpVector() * FMath::FRandRange(randomVerticalOffsetBounds.X, randomVerticalOffsetBounds.Y);

			if (!bIsProjectileCollide)
			{
				FHitResult hitInfo;
				bool doesHit = world->LineTraceSingleByChannel(hitInfo, origin, destination, ECC_GameTraceChannel1);
				if (bIsDebug)
				{
					DrawDebugLineTraceSingle(GetWorld(),
											 origin,
											 destination,
											 EDrawDebugTrace::ForDuration,
											 doesHit,
											 hitInfo,
											 FLinearColor::Red,
											 FLinearColor::Blue,
											 10.0f);
				}

				if (doesHit && hitInfo.GetActor() != character)
				{
					OnComponentHit.Broadcast(hitInfo.Component.Get(),
											 hitInfo.GetActor(),
											 hitInfo.GetComponent(),
											 hitInfo.ImpactNormal, 
											 hitInfo);
				}
			}


			if (projectileClass)
			{
				FRotator spawnRotation = (destination - origin).GetUnsafeNormal().Rotation();

				FActorSpawnParameters actorSpawnParams;
				actorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				world->SpawnActor<AActor>(projectileClass, origin, spawnRotation, actorSpawnParams);
			}
		}
	}

	if (attackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, attackSound, character->GetActorLocation());
	}

	if (attackAnimation)
	{
		UAnimInstance *animInstance = character->GetMesh()->GetAnimInstance();
		if (animInstance)
		{
			animInstance->Montage_Play(attackAnimation, 1.f);
		}
	}
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (character == nullptr)
	{
		return;
	}
	APlayerController *playerController = Cast<APlayerController>(character->GetController());
	if (playerController)
	{
		UEnhancedInputLocalPlayerSubsystem *subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->RemoveMappingContext(weaponMapping);
		}
	}
}

void UWeaponComponent::reset_cooldown()
{
	GetWorld()->GetTimerManager().ClearTimer(timer);
}

FVector UWeaponComponent::to_world_coordinates(const FVector &sphericalCoordinates) const
{
	FVector worldCoordinates;
	FVector2D radians;
	radians.X = FMath::DegreesToRadians(sphericalCoordinates.Y);
	radians.Y = FMath::DegreesToRadians(sphericalCoordinates.Z);
	worldCoordinates.Y = FMath::Sin(radians.X) * FMath::Cos(radians.Y);
	worldCoordinates.Z = FMath::Sin(radians.Y);
	worldCoordinates.X = FMath::Cos(radians.X);
	if (bIsWingDistributionAttack)
	{
		worldCoordinates.Z *= FMath::Sin(radians.X);
	}
	if (bIsAxisInverted)
	{
		Swap(worldCoordinates.Z, worldCoordinates.Y);
	}
	worldCoordinates.Normalize();
	worldCoordinates *= sphericalCoordinates.X;
	return worldCoordinates;
}
