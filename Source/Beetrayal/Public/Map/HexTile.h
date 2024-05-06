// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexTile.generated.h"

/** Direction that tile can lead to */
UENUM()
enum class ETileDirection : int8
{
	INVALID,
	UP,
	UPLEFT,
	UPRIGHT,
	DOWN = -UP,
	DOWNRIGHT = -UPLEFT,
	DOWNLEFT = -UPRIGHT
};

/** Operator to reverse direction, if D is Down it returns Up */
inline ETileDirection operator-(const ETileDirection& d)
{
	return (ETileDirection)(-(uint8)d);
}

// We use that hex orientation
//	 /\
//	|  |
//   \/
/** Hex grid coordinates */
struct FHexVector
{
	int32 a = 0; // Ascending coordinate axis by UpLeft to DownRight
	int32 c = 0; // Constant coordinate axis by Up to Down
	int32 d = 0; // Descending coordinate axis by UpRight to DownLeft

	FHexVector()
	{}

	FHexVector(int32 a, int32 c, int32 d)
		: a(a)
		, c(c)
		, d(d)
	{}

	FHexVector operator+(const FHexVector& vector) const
	{
		return FHexVector(a + vector.a, c + vector.c, d + vector.d);
	}

	FHexVector operator-(const FHexVector& vector) const
	{
		return FHexVector(a - vector.a, c - vector.c, d - vector.d);
	}

	FHexVector operator-() const
	{
		return FHexVector(-a, -c, -d);
	}

	bool operator==(const FHexVector& vector) const
	{
		return a == vector.a && c == vector.c && d == vector.d;
	}

	// Required for using in TSet
	friend uint32 GetTypeHash(const FHexVector& vector)
	{
		return FCrc::MemCrc_DEPRECATED(&vector, sizeof(vector));
	}

	FString to_string() const
	{
		return FString::Printf(TEXT("A=%d C=%d D=%d"), a, c, d);
	}
};

UCLASS()
class BEETRAYAL_API AHexTile : public AActor
{
	GENERATED_BODY()
public:
	FHexVector location;

protected:
	/** Hexagonal tile mesh used for random map generating */
	UPROPERTY(Category = "Tile", meta = (DisplayThumbnail = "true"), BlueprintReadWrite, EditDefaultsOnly)
	UStaticMeshComponent *tileMesh;

	/** List of ways that tile can lead to */
	UPROPERTY(Category = "Tile", EditAnywhere)
	TSet<ETileDirection> ways;

	/** Whether this tile have space in center for entrance */
	UPROPERTY(Category = "Tile|Marks", EditDefaultsOnly)
	bool bCanHaveEntrance;

	/** Whether this tile have space in center to place exit */
	UPROPERTY(Category = "Tile|Marks", EditDefaultsOnly)
	bool bCanHaveExit;

	/** Value used for spawning start and exit */
	UPROPERTY(Category = "Tile|Marks", EditDefaultsOnly)
	float floorHeight; // maybe_unused, I can't add this so I write it as a comment

public:
	AHexTile();

	/** Conversion from ETileDirection to FHexVector */
	static FHexVector direction_to_hex(ETileDirection direction);

	/** Conversion from FHexVector to ETileDirection */
	static ETileDirection hex_to_direction(const FHexVector &vector);

	/** Returns list of available ways */
	TArray<ETileDirection> get_available_ways() const;

	float get_floor_height() const;
	bool can_have_exit() const;
	bool can_have_entrance() const;

	/** Returns list of tile neighbours coordinates */
	static TArray<FHexVector> get_neighbours_grid_locations(const FHexVector &location);

};