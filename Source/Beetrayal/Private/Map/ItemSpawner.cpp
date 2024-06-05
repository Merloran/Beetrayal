#include "Map/ItemSpawner.h"

#include "Equipment/Item.h"

AItemSpawner::AItemSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void AItemSpawner::spawn_item()
{
	if (items.IsEmpty())
	{
		return;
	}

	FInt32Vector2 rollRange = { TNumericLimits<int32>::Max(), TNumericLimits<int32>::Min() };
	for (FItemPair &pair : items)
	{
		rollRange.X = FMath::Min(rollRange.X, pair.chance.X);
		rollRange.Y = FMath::Max(rollRange.Y, pair.chance.Y);
	}

	int32 rolled = FMath::RandRange(rollRange.X, rollRange.Y);

	TSubclassOf<AItem> itemClass;
	for (FItemPair &pair : items)
	{
		if (pair.chance.X <= rolled && pair.chance.Y >= rolled)
		{
			itemClass = pair.item;
		}
	}

	FTransform spawnTransform;
	spawnTransform.SetLocation(GetActorLocation());
	spawnTransform.SetScale3D(GetActorScale3D() * 0.1); // Somehow it is not working
	spawnTransform.SetRotation(GetActorRotation().Quaternion());

	AActor* spawned = GetWorld()->SpawnActor(itemClass, &spawnTransform);
	if (spawned)
	{
		spawned->SetActorScale3D(GetActorScale3D() * 0.1);
	}
}
