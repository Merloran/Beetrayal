// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExperienceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGainExperience, uint64, experience);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEETRAYAL_API UExperienceComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnLevelUp onLevelUp;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnGainExperience onGainExperience;

protected:
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	int64 ownedExperience;

private:
	static constexpr int64 EXPERIENCE_FOR_FIRST_LEVEL = 100;
	int32 level;

public:	
	UExperienceComponent();

	UFUNCTION(BlueprintCallable)
	void gain_experience(const int64 experience);

	UFUNCTION(BlueprintCallable)
	int64 get_current_experience() const;

	UFUNCTION(BlueprintCallable)
	int32 get_level() const;

	UFUNCTION(BlueprintCallable)
	void reset_level();

protected:
	virtual void BeginPlay() override;

private:
	void update_level();
};
