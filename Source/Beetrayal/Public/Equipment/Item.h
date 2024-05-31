// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class ARubee;

UCLASS(Blueprintable, BlueprintType)
class BEETRAYAL_API AItem : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Item")
	FString name;

	UPROPERTY(EditAnywhere, Category = "Item")
	int32 size;

	UPROPERTY(EditAnywhere, Category = "Item")
	bool bCanBeDropped;

	UPROPERTY(EditAnywhere, Category = "InvitingAnimation")
	bool bIsAnimated;

	UPROPERTY(EditAnywhere, Category = "InvitingAnimation")
	double rotationSpeed;

	UPROPERTY(EditAnywhere, Category = "InvitingAnimation")
	double scaleSpeed;

	UPROPERTY(EditAnywhere, Category = "InvitingAnimation")
	double movementOffset;

	UPROPERTY(EditAnywhere, Category = "InvitingAnimation")
	double movementSpeed;

private:
	bool bIsAnimationStarted;
	FVector beginPoint;

public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

	const FString &get_item_name();
	int32 get_item_size() const;
	bool can_be_dropped() const;

	UFUNCTION(BlueprintCallable)
	void animate_item(float deltaTime);

	UFUNCTION(BlueprintCallable)
	void reset_animation();

	UFUNCTION(BlueprintCallable)
	void set_animated(bool value);

	UFUNCTION(BlueprintNativeEvent)
	void pick(ARubee *character);

protected:
	virtual void BeginPlay() override;

private:
	void clamp_vector(FVector &vector, const FVector &min, const FVector &max);
};
