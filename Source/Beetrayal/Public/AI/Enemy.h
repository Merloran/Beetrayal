#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM()
enum class EEnemyType : int8
{
	None = 0,
	Offensive = 1,
	Defensive = 2,
	Escaping  = 3,
	Grouping  = 4,
};

UENUM()
enum class EEnemyBehaviour : int8
{
	None = 0,
	Aggressive = 1,
	Neutral	   = 2,
};

UCLASS()
class BEETRAYAL_API AEnemy : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	EEnemyType type;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	EEnemyBehaviour behaviour;

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
