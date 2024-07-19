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
	if (Damage == 0.0f) 
	{
		return;
	}

	AActor *causer = nullptr;
	if (InstigatedBy)
	{
		causer = InstigatedBy->GetPawn();
	}

	if (Damage > 0.0f)
	{
		injure(FMath::Abs(Damage), causer);
	}
	else if (currentHealth != maxHealth) 
	{
		heal(FMath::Abs(Damage), causer);
	}

	if (currentHealth == 0.0) 
	{
		on_zero_health(causer);
	}
}

void UHealthComponent::on_zero_health(AActor *causer) const
{
	onDeath.Broadcast(causer);
}

void UHealthComponent::heal(double amount, AActor* causer)
{
	currentHealth = FMath::Clamp(currentHealth + amount, 0.0, maxHealth);
	onHeal.Broadcast(causer);
}

void UHealthComponent::injure(double amount, AActor* causer)
{
	currentHealth = FMath::Clamp(currentHealth - amount, 0.0, maxHealth);
	onInjure.Broadcast(causer);
}

void UHealthComponent::InstantKill(AActor *DamagedActor, UPrimitiveComponent *FHitComponent, FVector ShotFromDirection, const UDamageType *DamageType, AActor *DamageCauser)
{
	if (currentHealth <= 0.0)
	{
		return;
	}

	currentHealth = 0.0;
	on_zero_health(DamageCauser);
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