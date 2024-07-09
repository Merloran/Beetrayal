#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChargeAttackComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChargeHit, AActor*, otherActor, const FHitResult&, hitInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharge, const FVector&, location);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEETRAYAL_API UChargeAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnCharge onCharge;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnChargeHit onChargeHit;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float speed;

	/** Charge cooldown in milliseconds */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float chargeCooldownMs;

	/** Charge duration in milliseconds */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float chargeDurationMs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay")
	TObjectPtr<UPrimitiveComponent> collisionComponent;

private:
	/** Timer used with cooldown */
	FTimerHandle timerCooldown;

	/** Timer used with charge */
	FTimerHandle timerCharge;

	TObjectPtr<ACharacter> owner;

	/** Cached walk speed of owner */
	float defaultSpeed;

public:	
	UChargeAttackComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool start_charge(const FVector& location);


	UFUNCTION()
	void on_charge_hit(UPrimitiveComponent *OverlappedComp, 
					 AActor *OtherActor,
					 UPrimitiveComponent *OtherComp, 
					 int32 OtherBodyIndex,
					 bool bFromSweep, 
					 const FHitResult &SweepResult);

	void turn_to_direction(const FVector &direction);

protected:
	virtual void BeginPlay() override;

	void end_charge();

private:
	void reset_cooldown();
};
