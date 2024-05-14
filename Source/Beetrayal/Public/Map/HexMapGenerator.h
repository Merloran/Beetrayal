// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include <Math/RandomStream.h>
#include "Map/HexTile.h"

#include "HexMapGenerator.generated.h"

class ANavMeshBoundsVolume;

DECLARE_LOG_CATEGORY_EXTERN(LogHexMap, Log, All);

UCLASS()
class BEETRAYAL_API AHexMapGenerator : public AActor
{
	GENERATED_BODY()

protected:
	/** List of using tiles to generate */
	UPROPERTY(Category = "Map|Tiles", EditAnywhere)
	TArray<TSubclassOf<AHexTile>> assemblyTiles;

	/** Tile to close all ways when there's not correct tile to place */
	UPROPERTY(Category = "Map|Tiles", EditAnywhere)
	TSubclassOf<AHexTile> noWayTile;

	/** Exit to spawn on tile */
	UPROPERTY(Category = "Map|Tiles", EditAnywhere)
	TSubclassOf<AActor> exitObject;

	/** Defines map generating mode FIFO: true, LIFO: false */
	UPROPERTY(Category = "Map", EditAnywhere)
	bool bIsInDepthMap;

	/** Expected count of tiles */
	UPROPERTY(Category = "Map", EditAnywhere)
	int32 mapSize;

	/** Distance between center and hexagon vertex */
	UPROPERTY(Category = "Map", EditAnywhere)
	double tileSize;

	/** It defines where */
	UPROPERTY(Category = "Map", EditAnywhere)
	FVector2D tileVerticalBounds;

	/** Reference to bounding volume that will be used for AI and must be adjusted to bound whole map */
	UPROPERTY(Category = "Map", EditAnywhere)
	TObjectPtr<ANavMeshBoundsVolume> volume;

private:
	/** Random generator and seed are used to make the generation reproducible in case of bugs */
	FRandomStream randomGenerator;
	int32 seed;

	/** Bounds that are used to adjust navigation volume */
	TStaticArray<FVector, 2> mapBounds;

	/** Collection of all spawned tiles */
	TMap<FHexVector, TObjectPtr<AHexTile>> tileMap;

	/** Tile where player start */
	TObjectPtr<AHexTile> entranceTile;

	/** Tile that contains ExitObject */
	TObjectPtr<AHexTile> exitTile;

	/** Offset to convert A coordinate of FHexVector to FVector */
	FVector offsetA;

	/** Offset to convert C coordinate of FHexVector to FVector */
	FVector offsetC;

	/** Offset to convert D coordinate of FHexVector to FVector */
	FVector offsetD;

public:
	AHexMapGenerator();

	/** Map getter */
	TMap<FHexVector, TObjectPtr<AHexTile>> &get_map();

	/** EntranceTile getter */
	TObjectPtr<AHexTile> get_entrance_tile();

	/** ExitTile getter */
	TObjectPtr<AHexTile> get_exit_tile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/** Returns tile set with only tiles that can have entrance */
	TArray<TSubclassOf<AHexTile>> select_entrance_tiles(const TArray<TSubclassOf<AHexTile>> &tileSet);

	AHexTile* find_tile_by_location(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>> &tilesToSpawn, const FHexVector &gridLocation);
	bool does_contain_location(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>> &tilesToSpawn, const FHexVector &gridLocation);

	/** Returns tile count depending on ring number */
	uint32 get_area_tiles_count(const uint32 ringNumber);

	/** Returns ring tile count */
	uint32 get_ring_tiles_count(const uint32 ringNumber);

	/** Returns number of ring that fits number of tiles given */
	int32 get_rings_count(const int32 tileCount);

	/** Returns distance between tile and position in hex grid */
	int32 get_tiles_distance(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>> &tilesToSpawn, 
							 const AHexTile& tile,
							 const FHexVector& gridPosition);

	/** Returns list of all coordinates on the ring with given number */
	TArray<FHexVector> get_ring_positions(const int32 ringNumber);

	/** It select tiles that can be placed in given location from given list and returns new list */
	TArray<TSubclassOf<AHexTile>> select_suitable_tiles(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>> &tilesToSpawn, 
														const TArray<TSubclassOf<AHexTile>> &tiles, 
														const FHexVector& location);

	/** Spawn exit on tile based on floor height */
	void spawn_exit(AHexTile& tile, const FVector& location);

	/** Returns array of tiles that have at least one way to space(not to another tile) */
	TArray<TSubclassOf<AHexTile>> select_open_tiles(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>>& tilesToSpawn,
	                                                const TArray<TSubclassOf<AHexTile>> &tiles,
	                                                const FHexVector &tileLocation);

	/** Generate random map based on given tiles */
	bool generate_map(bool Force = false);

	void spawn_map(const TArray<TPair<FHexVector, TSubclassOf<AHexTile>>>& tiles);

	/** Destroy all placed actors */
	void clear_map();

	/** Returns tiles without nullptrs */
	TArray<TSubclassOf<AHexTile>> select_valid_tiles(const TArray<TSubclassOf<AHexTile>> &tiles);

	void adjust_volume();
};