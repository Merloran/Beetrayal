// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HoneyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEETRAYAL_API UHoneyComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	double capacity;

	UPROPERTY(EditAnywhere)
	double currentAmount;

public:
	UHoneyComponent();

	UFUNCTION(BlueprintCallable)
	double get_capacity();

	UFUNCTION(BlueprintCallable)
	double get_current_amount();

	UFUNCTION(BlueprintCallable)
	void set_current_amount(double value);

	UFUNCTION(BlueprintCallable)
	void refill(double value);

	UFUNCTION(BlueprintCallable)
	void consume(double value);
};
