// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponComponent.generated.h"

class UInputMappingContext;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttack, ACharacter*, instigator);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEETRAYAL_API UWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

protected:
	/** Switch between ray cast and spawn projectile */
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	bool bIsProjectileCollide;

	/** Length of ray cast */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsProjectileCollide == false", EditConditionHides), Category = "Attack")
	double length;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<AActor> projectileClass;

	/** Value that defines number of projectiles/raycasts */
	UPROPERTY(EditAnywhere, Category = "Attack")
	FInt32Vector2 attackGrid;

	/** Degrees in horizontal and vertical span angles of attack grid */
	UPROPERTY(EditAnywhere, Category = "Attack")
	FVector2D degrees;

	/** Bounds that defines max and min values of random offset in horizontal axis, where positive value is offset to right */
	UPROPERTY(EditAnywhere, Category = "Attack")
	FVector2D randomHorizontalOffsetBounds;

	/** Bounds that defines max and min values of random offset in vertical axis, where positive value is offset to up */
	UPROPERTY(EditAnywhere, Category = "Attack")
	FVector2D randomVerticalOffsetBounds;

	/** Weapon offset from the characters location which defines where attack hit begins */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FVector originOffset;

	/** Enable drawing rays for ray cast */
	UPROPERTY(EditAnywhere, Category = "Attack")
	bool bIsDebug;

	/** Swap X with Y axes it is useful with wing distribution
	 
		False:
		 .     .
		. . . . .
		 .     .
		
		True:
		  .
		.   .
		  .
		  .
		  .
		.   .
		  .
	 */
	UPROPERTY(EditAnywhere, Category = "Attack")
	bool bIsAxisInverted;

	/** This variable switch type of attack distribution
	 
		False means that attack is distributed in 2D rectangular grid on sphere,
		where Y degrees defines angle between horizontal attack edge
		. . . .
		. . . .
		. . . .
		. . . .
		 
		True means that attack distribution make 2 wings,
		where Y degrees defines angle between wing up and down edges, e.g. 180 will make circle
		 .     .
		. . . . .
		 .     .
	*/
	UPROPERTY(EditAnywhere, Category = "Attack")
	bool bIsWingDistributionAttack;

	/** Attack cooldown in milliseconds */
	UPROPERTY(EditAnywhere, Category = "Attack")
	float attackCooldownMs;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TObjectPtr<USoundBase> attackSound;

	/** AnimMontage to play each time we attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TObjectPtr<UAnimMontage> attackAnimation;

	/** AnimMontage to play each time we hide weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TObjectPtr<UAnimMontage> hideAnimation;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> weaponMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> attackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> hideAction;

	UPROPERTY(BlueprintAssignable, Category = "Attack")
	FOnAttack onAttack;

private:
	/** The Character holding this weapon */
	TObjectPtr<ACharacter> character;

	/** Timer used with cooldown */
	FTimerHandle timer;
public:	
	UWeaponComponent();

	/** Attaches the actor to owner */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool attach(ACharacter* target);

	/** Make the weapon attack */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void attack();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void reset_cooldown();

	FVector to_world_coordinates(const FVector& sphericalCoordinates) const;
};
