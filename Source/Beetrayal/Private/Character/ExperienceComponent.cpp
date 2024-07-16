#include "Character/ExperienceComponent.h"


UExperienceComponent::UExperienceComponent()
	: ownedExperience(0)
	, level(0)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UExperienceComponent::gain_experience(const int64 experience)
{
	onGainExperience.Broadcast(experience);
	ownedExperience += experience;
	update_level();
}

int64 UExperienceComponent::get_current_experience() const
{
	return ownedExperience;
}

int32 UExperienceComponent::get_level() const
{
	return level;
}

void UExperienceComponent::reset_level()
{
	ownedExperience = 0;
	update_level();
}

void UExperienceComponent::BeginPlay()
{
	Super::BeginPlay();

	update_level();
}

void UExperienceComponent::update_level()
{
	int32 previousLevel = level;
	level = FMath::Log2(double(int64(ownedExperience * 2 / EXPERIENCE_FOR_FIRST_LEVEL)));

	if (level > previousLevel)
	{
		onLevelUp.Broadcast();
	}
}
