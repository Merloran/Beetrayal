#include "Map/Spawner.h"

ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!Enemies.IsEmpty())
	{
		TSubclassOf<AActor> Enemy = Enemies[FMath::RandRange(0, Enemies.Num() - 1)];

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation());
		SpawnTransform.SetScale3D(GetActorScale3D());
		SpawnTransform.SetRotation(GetActorRotation().Quaternion());

		AActor *SpawnedEnemy = GetWorld()->SpawnActor(Enemy, &SpawnTransform);
		//AActor* Temp = GetParentActor();
		//if (IsValid(Temp))
		//{
		//	SpawnedEnemy->AttachToActor(Temp, FAttachmentTransformRules::KeepWorldTransform);
		//}
	}
}

