#include "AI/Enemy.h"

AEnemy::AEnemy()
	: type(EEnemyType::None)
	, behaviour(EEnemyBehaviour::None)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}