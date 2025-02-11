// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, deathCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEETRAYAL_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDeath onDeath;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDeath onInjure;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDeath onHeal;

protected:
	UPROPERTY(EditAnywhere)
	double maxHealth;

	UPROPERTY(VisibleAnywhere)
	double currentHealth;

public:
	UHealthComponent();

	UFUNCTION(BlueprintCallable)
	void heal(double amount, AActor* causer = nullptr);

	UFUNCTION(BlueprintCallable)
	void injure(double amount, AActor* causer = nullptr);

	UFUNCTION(BlueprintCallable)
	void TakeDamage(AActor *DamagedActor, 
					float Damage, 
					AController *InstigatedBy, 
					FVector HitLocation, 
					UPrimitiveComponent *FHitComponent,
					FName BoneName, 
					FVector ShotFromDirection, 
					const UDamageType *DamageType, 
					AActor *DamageCauser);

	void InstantKill(AActor *DamagedActor, 
					 UPrimitiveComponent *FHitComponent, 
					 FVector ShotFromDirection,
					 const UDamageType *DamageType, 
					 AActor *DamageCauser);

	UFUNCTION(BlueprintCallable)
	double get_max_health();

	UFUNCTION(BlueprintCallable)
	double get_current_health();

	void set_current_health(double value);

protected:
	virtual void BeginPlay() override;

	void on_zero_health(AActor* causer = nullptr) const;

		
};
