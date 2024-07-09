#include "AI/ChargeAttackComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UChargeAttackComponent::UChargeAttackComponent()
    : speed(0.0f)
    , chargeCooldownMs(0.0f)
    , chargeDurationMs(0.0f)
    , defaultSpeed(0.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UChargeAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UWorld *const world = GetWorld();
    if (!world || !owner)
    {
        return;
    }

    FTimerManager &timerManager = world->GetTimerManager();
    if (timerManager.IsTimerActive(timerCharge))
    {
        owner->AddMovementInput(owner->GetActorForwardVector(), 1.0f);
    }
}

bool UChargeAttackComponent::start_charge(const FVector& location)
{
    UWorld *const world = GetWorld();

    if (!world)
    {
        return false;
    }

    FTimerManager &timerManager = world->GetTimerManager();
    if (timerManager.IsTimerActive(timerCharge) || timerManager.IsTimerActive(timerCooldown))
    {
        return false;
    }

    GetWorld()->GetTimerManager().SetTimer(timerCharge, 
                                           this, 
                                           &UChargeAttackComponent::end_charge,
                                           chargeDurationMs * 0.001f);

    if (collisionComponent)
    {
        collisionComponent->OnComponentBeginOverlap.AddDynamic(this, &UChargeAttackComponent::on_charge_hit);
    }


    if (owner)
    {
        defaultSpeed = owner->GetCharacterMovement()->MaxWalkSpeed;
        owner->GetCharacterMovement()->MaxWalkSpeed = speed;
        turn_to_direction((location - owner->GetActorLocation()));
    }

    return true;
}

void UChargeAttackComponent::on_charge_hit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && owner && OtherActor != owner)
    {
        onChargeHit.Broadcast(OtherActor, SweepResult);
    }

    if (collisionComponent)
    {
        collisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UChargeAttackComponent::on_charge_hit);
    }
}

void UChargeAttackComponent::turn_to_direction(const FVector& direction)
{
    if (!owner)
    {
        return;
    }
    FRotator targetRotation = FRotationMatrix::MakeFromX({ direction.X, direction.Y, 0.0f }).Rotator();

    owner->SetActorRotation(targetRotation);
}

void UChargeAttackComponent::BeginPlay()
{
	Super::BeginPlay();
    owner = GetOwner<ACharacter>();
    if (owner)
    {
        collisionComponent->AttachToComponent(owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    }
}

void UChargeAttackComponent::end_charge()
{
    UWorld *const world = GetWorld();

    if (!world)
    {
        return;
    }

    FTimerManager &timerManager = world->GetTimerManager();

    timerManager.SetTimer(timerCooldown,
                          this,
                          &UChargeAttackComponent::reset_cooldown,
                          chargeCooldownMs * 0.001f);


    owner->GetCharacterMovement()->MaxWalkSpeed = defaultSpeed;

    timerManager.ClearTimer(timerCharge);
}

void UChargeAttackComponent::reset_cooldown()
{
    GetWorld()->GetTimerManager().ClearTimer(timerCooldown);
}

