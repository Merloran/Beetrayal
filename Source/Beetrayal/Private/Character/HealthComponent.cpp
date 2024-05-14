// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/HealthComponent.h"

UHealthComponent::UHealthComponent()
	: maxHealth(100)
	, currentHealth(maxHealth)
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UHealthComponent::BeginPlay()
{
	currentHealth = maxHealth;
	AActor *Owner = GetOwner();

	Owner->OnTakePointDamage.AddDynamic(this, &UHealthComponent::TakeDamage);

	Super::BeginPlay();
}

void UHealthComponent::TakeDamage(AActor *DamagedActor, float Damage, AController *InstigatedBy, FVector HitLocation, UPrimitiveComponent *FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType *DamageType, AActor *DamageCauser)
{
	onTakenDamage.Broadcast();
	if (Damage == 0) 
	{
		return;
	}

	currentHealth = FMath::Clamp(currentHealth - Damage, 0, maxHealth);

	if (currentHealth == 0) 
	{
		OnZeroHP();
	}
}

void UHealthComponent::OnZeroHP() const
{
	onDeath.Broadcast();
}

void UHealthComponent::heal(double amount)
{
	currentHealth = FMath::Clamp((currentHealth + amount), 0, maxHealth);;
}

void UHealthComponent::InstantKill(AActor *DamagedActor, UPrimitiveComponent *FHitComponent, FVector ShotFromDirection, const UDamageType *DamageType, AActor *DamageCauser)
{
	if (currentHealth <= 0)
	{
		return;
	}

	currentHealth = 0;
	OnZeroHP();
}

double UHealthComponent::get_max_health()
{
	return maxHealth;
}

double UHealthComponent::get_current_health()
{
	return currentHealth;
}

void UHealthComponent::set_current_health(double value)
{
	currentHealth = value;
}

