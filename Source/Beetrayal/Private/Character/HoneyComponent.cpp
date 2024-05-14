#include "Character/HoneyComponent.h"

UHoneyComponent::UHoneyComponent()
	: capacity(100.0)
	, currentAmount(100.0)
{
	PrimaryComponentTick.bCanEverTick = false;
}


double UHoneyComponent::get_capacity()
{
	return capacity;
}

double UHoneyComponent::get_current_amount()
{
	return currentAmount;
}

void UHoneyComponent::set_current_amount(double value)
{
	currentAmount = value;
}

void UHoneyComponent::refill(double value)
{
	currentAmount = FMath::Clamp(currentAmount + value, 0.0, capacity);
}

void UHoneyComponent::consume(double value)
{
	currentAmount = FMath::Clamp(currentAmount - value, 0.0, capacity);
}

