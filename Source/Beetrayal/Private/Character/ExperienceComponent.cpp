#include "Character/ExperienceComponent.h"


UExperienceComponent::UExperienceComponent()
	: currentExperience(0)
	, level(0)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UExperienceComponent::gain_experience(const uint64 experience)
{
	currentExperience += experience;
	update_level();
}

uint64 UExperienceComponent::get_current_experience() const
{
	return currentExperience;
}

uint32 UExperienceComponent::get_level() const
{
	return level;
}

void UExperienceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UExperienceComponent::update_level()
{
	level = FMath::Log2(double(uint64(currentExperience / EXPERIENCE_FOR_FIRST_LEVEL)));
}
