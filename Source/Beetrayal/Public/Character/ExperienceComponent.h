// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExperienceComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEETRAYAL_API UExperienceComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	static constexpr uint64 EXPERIENCE_FOR_FIRST_LEVEL = 100;
	uint64 currentExperience;
	uint32 level;

public:	
	UExperienceComponent();

	void gain_experience(const uint64 experience);
	uint64 get_current_experience() const;
	uint32 get_level() const;

protected:
	virtual void BeginPlay() override;

private:
	void update_level();
};
