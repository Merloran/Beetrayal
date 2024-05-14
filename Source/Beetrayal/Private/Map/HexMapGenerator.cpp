#include "Map/HexMapGenerator.h"

#include <Containers/Deque.h>
#include <NavMesh/NavMeshBoundsVolume.h>

#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"

DEFINE_LOG_CATEGORY(LogHexMap);

AHexMapGenerator::AHexMapGenerator()
	: bIsInDepthMap(false)
	, mapSize(0)
	, tileSize(1200.0)
	, tileVerticalBounds(-50.0, 500.0)
	, seed(0)
	, entranceTile(nullptr)
	, exitTile(nullptr)
	, volume(nullptr)
{
	mapBounds[0] = FVector(TNumericLimits<double>::Max());
	mapBounds[1] = FVector(-TNumericLimits<double>::Max());
}

void AHexMapGenerator::BeginPlay()
{
	Super::BeginPlay();
	offsetA = FVector(tileSize * 0.75, tileSize * DOUBLE_UE_SQRT_3, 0.0);
	offsetC = FVector(-tileSize * 0.75, tileSize * DOUBLE_UE_HALF_SQRT_3, 0.0);
	offsetD = FVector(tileSize * 0.75, 0.0, 0.0);
	FMatrix rotationMatrix = FRotationMatrix(GetActorRotation());

	// Rotacja wektorów offsetów
	offsetA = rotationMatrix.TransformVector(offsetA);
	offsetC = rotationMatrix.TransformVector(offsetC);
	offsetD = rotationMatrix.TransformVector(offsetD);

	FVector scale = GetActorScale3D();
	offsetA *= scale;
	offsetC *= scale;
	offsetD *= scale;

	assemblyTiles = select_valid_tiles(assemblyTiles);
	for (int8 i = 0; i < 16; ++i) // Try to generate expected map size with exit
	{
		seed = FMath::Rand32();
		randomGenerator.Initialize(seed);

		if (generate_map())
		{
			UE_LOG(LogHexMap, Display, TEXT("Map generated with seed: %d"), seed);
			break;
		}
		
		if (i == 15)
		{
			UE_LOG(LogHexMap, Display, TEXT("Map generated with seed: %d"), seed);
			generate_map(true);
		}
	}

	if (IsValid(volume))
	{
		adjust_volume();
	}

}

TArray<TSubclassOf<AHexTile>> AHexMapGenerator::select_entrance_tiles(const TArray<TSubclassOf<AHexTile>> &tileSet)
{
	TArray<TSubclassOf<AHexTile>> result;
	for (const TSubclassOf<AHexTile> &tileSub : tileSet)
	{
		AHexTile& tile = *tileSub->GetDefaultObject<AHexTile>();

		if (tile.can_have_entrance())
		{
			result.Add(tileSub);
		}
	}
	return result;
}

AHexTile* AHexMapGenerator::find_tile_by_location(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>>& tilesToSpawn, const FHexVector& gridLocation)
{
	for (const TPair<FHexVector, TSubclassOf<AHexTile>>& pair : tilesToSpawn)
	{
		if (pair.Key == gridLocation)
		{
			AHexTile *tile = pair.Value.GetDefaultObject();
			tile->location = pair.Key;
			return tile;
		}
	}
	return nullptr;
}

bool AHexMapGenerator::does_contain_location(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>>& tilesToSpawn, const FHexVector& gridLocation)
{
	for (const TPair<FHexVector, TSubclassOf<AHexTile>>& tile : tilesToSpawn)
	{
		if (tile.Key == gridLocation)
		{
			return true;
		}
	}

	return false;
}

uint32 AHexMapGenerator::get_area_tiles_count(const uint32 ringNumber)
{
	if (ringNumber == 1)
	{
		return 1;
	}

	if (ringNumber == 0)
	{
		return 0;
	}

	return get_area_tiles_count(ringNumber - 1) + 6 * (ringNumber - 1);
}

uint32 AHexMapGenerator::get_ring_tiles_count(const uint32 ringNumber)
{
	if (ringNumber == 1)
	{
		return 1;
	}

	if (ringNumber == 0)
	{
		return 0;
	}

	return 6 * (ringNumber - 1);
}

int32 AHexMapGenerator::get_rings_count(const int32 tileCount)
{
	if (tileCount <= 1)
	{
		return 0;
	}

	int32 result, sum = 1;

	for (result = 2; sum < tileCount; ++result)
	{
		sum += get_ring_tiles_count(result);
	}

	return result;
}

int32 AHexMapGenerator::get_tiles_distance(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>> &tilesToSpawn, const AHexTile &tile, const FHexVector &gridPosition)
{
	TSet<FHexVector> visited;

	TFunctionRef<int32(const AHexTile&, const FHexVector&, int32)> calculate =
		[&](const AHexTile &tile, const FHexVector &gridPosition, int32 distance)
	{
		visited.Add(tile.location);

		int32 result = 0;
		for (ETileDirection way : tile.get_available_ways())
		{
			FHexVector location = tile.location + AHexTile::direction_to_hex(way);
			if (location == gridPosition)
			{
				return distance;
			}

			if (!visited.Contains(location))
			{
				AHexTile* neighbour = find_tile_by_location(tilesToSpawn, location);
				if (neighbour != nullptr && !neighbour->get_available_ways().IsEmpty())
				{
					result = calculate(*neighbour, gridPosition, distance + 1);
					if (result != 0)
					{
						break;
					}
				}
			}
		}

		return result;
	};

	const int32 result = calculate(tile, gridPosition, 1);

	visited.Empty();

	return result;
}

TArray<FHexVector> AHexMapGenerator::get_ring_positions(const int32 ringNumber)
{
	TArray<FHexVector> result;

	for (int32 i = 0; i <= ringNumber; ++i)
	{
		result.AddUnique(FHexVector(ringNumber, -ringNumber + i, -i));
		result.AddUnique(FHexVector(-ringNumber, ringNumber - i, i));

		result.AddUnique(FHexVector(i, -ringNumber, ringNumber - i));
		result.AddUnique(FHexVector(-i, ringNumber, -ringNumber + i));

		result.AddUnique(FHexVector(-i, -ringNumber + i, ringNumber));
		result.AddUnique(FHexVector(i, ringNumber - i, -ringNumber));
	}
	return result;
}

TArray<TSubclassOf<AHexTile>> AHexMapGenerator::select_suitable_tiles(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>> &tilesToSpawn, const TArray<TSubclassOf<AHexTile>> &tiles, const FHexVector &location)
{
	TArray<TSubclassOf<AHexTile>> result;

	TArray<ETileDirection> openWays; // List of required ways for tiles
	TArray<ETileDirection> closeWays; // List of forbidden ways for tiles


	for (FHexVector& neighbourLocation : AHexTile::get_neighbours_grid_locations(location)) // Check all neighbors
	{
		ETileDirection direction = AHexTile::hex_to_direction(location - neighbourLocation); // Get way from neighbor to current location

		for (const TPair<FHexVector, TSubclassOf<AHexTile>>& pair : tilesToSpawn)
		{
			if (pair.Key == neighbourLocation) // Neighbor is placed on map
			{
				AHexTile &tile = *pair.Value.GetDefaultObject();
				if (tile.get_available_ways().Contains(direction)) // Neighbor have way to connect with
				{
					openWays.Add(-direction);
				} else { // Neighbor don't have way to connect with
					closeWays.Add(-direction);
				}
				break;
			}
		}
	}

	for (const TSubclassOf<AHexTile>& tileSub : tiles)
	{
		AHexTile* tile = tileSub->GetDefaultObject<AHexTile>();
		if (tile != nullptr)
		{
			bool bIsTileCorrect = true;
			TArray<ETileDirection> ways = tile->get_available_ways();

			/** Whether tile have ways to connect with all neighbors */
			for (ETileDirection way : openWays)
			{
				if (!ways.Contains(way))
				{
					bIsTileCorrect = false;
					break;
				}
			}

			if (!bIsTileCorrect) // Check next tile if this tile is not correct
			{
				continue;
			}

			/** Whether tile don't have way into wall */
			for (ETileDirection way : closeWays)
			{
				if (ways.Contains(way))
				{
					bIsTileCorrect = false;
					break;
				}
			}

			if (bIsTileCorrect)
			{
				result.Add(tileSub);
			}
		}
	}

	return result;
}

void AHexMapGenerator::spawn_exit(AHexTile &tile, const FVector &location)
{
	FTransform spawnTransform;

	spawnTransform.SetLocation(location + FVector(0, 0, tile.get_floor_height()));
	spawnTransform.SetRotation(GetActorRotation().Quaternion());
	spawnTransform.SetScale3D(GetActorScale3D());

	AActor &exitRef = *GetWorld()->SpawnActor(exitObject, &spawnTransform);
	exitRef.AttachToActor(&tile, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
}

TArray<TSubclassOf<AHexTile>> AHexMapGenerator::select_open_tiles(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>>& tilesToSpawn, const TArray<TSubclassOf<AHexTile>>& tiles, const FHexVector &tileLocation)
{
	TArray<TSubclassOf<AHexTile>> result;

	TArray<FHexVector> neighbours;

	for (const TSubclassOf<AHexTile> &tileSub : tiles)
	{
		AHexTile& tile = *tileSub->GetDefaultObject<AHexTile>();
		for (const ETileDirection way : tile.get_available_ways())
		{
			if (!does_contain_location(tilesToSpawn, tileLocation + AHexTile::direction_to_hex(way)))
			{
				result.Add(tileSub);
				break;
			}
		}
	}

	return result;
}

bool AHexMapGenerator::generate_map(bool Force)
{
	TDeque<FHexVector> spawnOrder;
	TSet<FHexVector> calledVectors;

	TArray<TPair<FHexVector, TSubclassOf<AHexTile>>> tilesToSpawn;
	int64 predictedMapSize = 3 * mapSize; // Predicted size of map including NoWay tiles
	tilesToSpawn.Reserve(predictedMapSize);
	TArray<TSubclassOf<AHexTile>> availableTiles;

	FHexVector gridLocation = FHexVector(0, 0, 0);
	bool willExitSpawn = false;

	calledVectors.Add(gridLocation);

	TSubclassOf<AHexTile> tile;
	AHexTile* entrance = nullptr;

	// Value to limit NoWayTiles count
	const int32 minimumOrderNum = mapSize * 0.03;
	const int32 minimumDistance = get_rings_count(mapSize);
	const int32 minimumTileNumber = mapSize * 0.8;

	if (assemblyTiles.IsEmpty() || mapSize <= 0)
	{
		return false;
	}

	for (int32 i = 0; i < mapSize; ++i)
	{
		if (entrance != nullptr)
		{
			availableTiles = select_suitable_tiles(tilesToSpawn, assemblyTiles, gridLocation);
		} else {
			availableTiles = select_entrance_tiles(assemblyTiles);
		}

		if (tilesToSpawn.Num() < mapSize - 1 && spawnOrder.Num() < minimumOrderNum)
		{
			availableTiles = select_open_tiles(tilesToSpawn, availableTiles, gridLocation);
		}

		if (!availableTiles.IsEmpty()) // Select random tile
		{
			tile = availableTiles[randomGenerator.RandRange(0, availableTiles.Num() - 1)];
		} else { // If no tiles fulfill conditions place wall
			tile = noWayTile;
			--i;
		}


		AHexTile &tempTile = *tile.GetDefaultObject();
		if (entrance != nullptr
			&& tilesToSpawn.Num() >= minimumTileNumber
			&& tempTile.can_have_exit()
			&& get_tiles_distance(tilesToSpawn, *entrance, gridLocation) >= minimumDistance)
		{
			willExitSpawn = true;
		}

		if (entrance == nullptr)
		{
			entrance = &tempTile;
			entrance->location = gridLocation;
		}

		tilesToSpawn.Add({ gridLocation, tile });

		// Enqueue all possible to connect neighbors
		for (const ETileDirection Dir : tempTile.get_available_ways())
		{
			FHexVector neighbourLocation = gridLocation + AHexTile::direction_to_hex(Dir);
			if (!calledVectors.Contains(neighbourLocation))
			{
				calledVectors.Add(neighbourLocation);
				bIsInDepthMap ? spawnOrder.PushLast(neighbourLocation) : spawnOrder.PushFirst(neighbourLocation);
			}
		}

		if (spawnOrder.IsEmpty())
		{
			break;
		}

		gridLocation = spawnOrder.Last();
		spawnOrder.PopLast();
	}

	while (!spawnOrder.IsEmpty())
	{
		tilesToSpawn.Add({ gridLocation, noWayTile });

		gridLocation = spawnOrder.Last();
		spawnOrder.PopLast();
	}
	tilesToSpawn.Add({ gridLocation, noWayTile }); // Add last poped tile

	if (willExitSpawn && tilesToSpawn.Num() >= mapSize || Force)
	{
		spawn_map(tilesToSpawn);
		return true;
	}
	return false;
}

void AHexMapGenerator::spawn_map(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>> &tiles)
{
	FTransform spawnTransform;
	const FVector &mapOrigin = GetActorLocation();
	const int32 minimumDistance = get_rings_count(mapSize);
	const int32 minimumTileNumber = mapSize * 0.8;

	for (const TPair<FHexVector, TSubclassOf<AHexTile>> &pair : tiles)
	{
		FHexVector gridLocation = pair.Key;
		TSubclassOf<AHexTile> tile = pair.Value;

		FVector inWorldLocation = mapOrigin + offsetA * gridLocation.a + offsetC * gridLocation.c + offsetD * gridLocation.d;
		spawnTransform.SetLocation(inWorldLocation);
		spawnTransform.SetRotation(GetActorRotation().Quaternion());
		spawnTransform.SetScale3D(GetActorScale3D());

		mapBounds[0] = FVector::Min(inWorldLocation, mapBounds[0]);
		mapBounds[1] = FVector::Max(inWorldLocation, mapBounds[1]);

		AHexTile& spawnedTile = *GetWorld()->SpawnActorDeferred<AHexTile>(tile, spawnTransform);
		spawnedTile.location = gridLocation;
		spawnedTile.FinishSpawning(spawnTransform);

		tileMap.Add(gridLocation, spawnedTile);

		if (entranceTile == nullptr)
		{
			entranceTile = spawnedTile;
		}

		if (exitTile == nullptr && exitObject != nullptr)
		{
			if (tileMap.Num() >= minimumTileNumber
				&& spawnedTile.can_have_exit()
				&& get_tiles_distance(tiles, *entranceTile, gridLocation) >= minimumDistance)
			{
				spawn_exit(spawnedTile, spawnTransform.GetLocation());
				exitTile = spawnedTile;
			}
		}
	}
}

void AHexMapGenerator::clear_map()
{
	TArray<AActor*> actorsToDestroy;
	for (const TPair<FHexVector, TObjectPtr<AHexTile>>& pair : tileMap)
	{
		TObjectPtr<AHexTile> tile = pair.Value;
		tile->GetAttachedActors(actorsToDestroy);
		for (AActor* Actor : actorsToDestroy)
		{
			if (Actor != nullptr)
			{
				Actor->Destroy();
			}
		}
		tile->Destroy();
	}

	if (entranceTile != nullptr)
	{
		entranceTile->Destroy();
		entranceTile = nullptr;
	}

	if (exitTile != nullptr)
	{
		exitTile->Destroy();
		exitTile = nullptr;
	}
}

TArray<TSubclassOf<AHexTile>> AHexMapGenerator::select_valid_tiles(const TArray<TSubclassOf<AHexTile>> &tiles)
{
	TArray<TSubclassOf<AHexTile>> result;
	result.Reserve(tiles.Num());
	for (const TSubclassOf<AHexTile>& tileSub : tiles)
	{
		if (tileSub != nullptr)
		{
			result.Add(tileSub);
		}
	}
	return result;
}

void AHexMapGenerator::adjust_volume()
{
	//TODO: think of calculate it for 1 tile too
	FVector origin  = GetActorLocation();
	FVector toEnd   = mapBounds[1] - origin;
	FVector toBegin = mapBounds[0] - origin;
	bool isOriginEnd   = !toEnd.Normalize();
	bool isOriginBegin = !toBegin.Normalize();
	bool isOneTileMap  = isOriginBegin && isOriginEnd; // One tile should not have monsters

	if (isOneTileMap)
	{
		return;
	}

	if (isOriginEnd)
	{
		toEnd = -toBegin;
	}
	else if (isOriginBegin)
	{
		toBegin = -toEnd;
	}

	FVector scale = GetActorScale3D();
	FVector up = GetActorUpVector();
	const double halfSqrtSix = FMath::Sqrt(6.0) * 0.5;
	mapBounds[0] += (toBegin * tileSize * halfSqrtSix + up * tileVerticalBounds.X) * scale;
	mapBounds[1] += (toEnd * tileSize * halfSqrtSix + up * tileVerticalBounds.Y) * scale;

	FVector volumeScale = (mapBounds[1] - mapBounds[0]) * 0.5;
	FVector mapCenter = mapBounds[0] + volumeScale;

	volume->SetActorLocation(mapCenter);
	constexpr double invVolumeBaseSize = 1.0 / 50.0;
	volume->SetActorScale3D(volumeScale * invVolumeBaseSize);

	FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->OnNavigationBoundsUpdated(volume);
}


/*
if (GEngine)
GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString("Tiles not empty"));

UE_LOG(LogTemp, Warning, TEXT("%s"), *v.ToString());
*/




TMap<FHexVector, TObjectPtr<AHexTile>> &AHexMapGenerator::get_map()
{
	return tileMap;
}

TObjectPtr<AHexTile> AHexMapGenerator::get_entrance_tile()
{
	return entranceTile;
}

TObjectPtr<AHexTile> AHexMapGenerator::get_exit_tile()
{
	return exitTile;
}