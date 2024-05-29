// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Item.h"

// Sets default values
AItem::AItem()
	: name("None")
	, size(1)
	, bCanBeDropped(true)
	, bIsAnimated(false)
	, rotationSpeed(10.0)
	, scaleSpeed(0.1)
	, movementOffset(100.0)
	, movementSpeed(10.0)
	, bIsAnimationStarted(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (bIsAnimated)
	{
		animate_item(DeltaTime);
	}
}


const FString &AItem::get_item_name()
{
	return name;
}

int32 AItem::get_item_size() const
{
	return size;
}

bool AItem::can_be_dropped() const
{
	return bCanBeDropped;
}

void AItem::animate_item(float deltaTime)
{
	if (!bIsAnimationStarted)
	{
		bIsAnimationStarted = true;
		beginPoint = GetActorLocation();
	}

	AddActorWorldRotation(FQuat(GetActorUpVector(), FMath::DegreesToRadians(rotationSpeed)));
	FVector scale = GetActorScale3D();

	scale += FVector(scaleSpeed * deltaTime);
	clamp_vector(scale, FVector(0.0), FVector(1.0));
	SetActorScale3D(scale);

	double length    = (GetActorLocation() - beginPoint).Length();
	FVector position = GetActorLocation();
	if (length >= movementOffset)
	{
		movementSpeed *= -1.0;
	}
	position		+= GetActorUpVector() * movementSpeed * deltaTime;

	clamp_vector(position, beginPoint - GetActorUpVector() * movementOffset, beginPoint + GetActorUpVector() * movementOffset);
	SetActorLocation(position);
}

void AItem::reset_animation()
{
	bIsAnimationStarted = false;
}

void AItem::set_animated(bool value)
{
	bIsAnimated = value;
}

void AItem::clamp_vector(FVector &vector, const FVector &min, const FVector &max)
{
	vector.X = FMath::Clamp(vector.X, min.X, max.X);
	vector.Y = FMath::Clamp(vector.Y, min.Y, max.Y);
	vector.Z = FMath::Clamp(vector.Z, min.Z, max.Z);
}
