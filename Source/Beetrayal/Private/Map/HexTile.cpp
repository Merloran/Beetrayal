#include "Map/HexTile.h"

// Sets default values
AHexTile::AHexTile()
	: bCanHaveEntrance(true)
	, bCanHaveExit(true)
	, floorHeight(46.0)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	tileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	tileMesh->SetupAttachment(RootComponent);

	ways.Add(ETileDirection::UP);
	ways.Add(ETileDirection::UPLEFT);
	ways.Add(ETileDirection::UPRIGHT);
	ways.Add(ETileDirection::DOWN);
	ways.Add(ETileDirection::DOWNLEFT);
	ways.Add(ETileDirection::DOWNRIGHT);
}

FHexVector AHexTile::direction_to_hex(ETileDirection direction)
{
	FHexVector Result = FHexVector(0, 0, 0);
	switch (direction)
	{
		case ETileDirection::UP:
		{
			Result = FHexVector(1, 0, -1);
			break;
		}
		case ETileDirection::DOWN:
		{
			Result = FHexVector(-1, 0, 1);
			break;
		}
		case ETileDirection::UPLEFT:
		{
			Result = FHexVector(1, -1, 0);
			break;
		}
		case ETileDirection::DOWNRIGHT:
		{
			Result = FHexVector(-1, 1, 0);
			break;
		}
		case ETileDirection::UPRIGHT:
		{
			Result = FHexVector(0, 1, -1);
			break;
		}
		case ETileDirection::DOWNLEFT:
		{
			Result = FHexVector(0, -1, 1);
			break;
		}
		case ETileDirection::INVALID:
		default:
		break;
	}
	return Result;
}

ETileDirection AHexTile::hex_to_direction(const FHexVector &vector)
{
	ETileDirection Result = ETileDirection::INVALID;

	if (vector == FHexVector(1, 0, -1))
	{
		Result = ETileDirection::UP;
	} else if (vector == FHexVector(-1, 0, 1))
	{
		Result = ETileDirection::DOWN;
	} else if (vector == FHexVector(0, 1, -1))
	{
		Result = ETileDirection::UPRIGHT;
	} else if (vector == FHexVector(0, -1, 1))
	{
		Result = ETileDirection::DOWNLEFT;
	} else if (vector == FHexVector(1, -1, 0))
	{
		Result = ETileDirection::UPLEFT;
	} else if (vector == FHexVector(-1, 1, 0))
	{
		Result = ETileDirection::DOWNRIGHT;
	}

	return Result;
}

TArray<ETileDirection> AHexTile::get_available_ways() const
{
	TArray<ETileDirection> Result;

	for (ETileDirection Way : ways)
	{
		Result.Add(Way);
	}

	return Result;
}

float AHexTile::get_floor_height() const
{
	return floorHeight;
}

bool AHexTile::can_have_exit() const
{
	return bCanHaveExit;
}

bool AHexTile::can_have_entrance() const
{
	return bCanHaveEntrance;
}

TArray<FHexVector> AHexTile::get_neighbours_grid_locations(const FHexVector &location)
{
	TArray<FHexVector> Neighbours;
	Neighbours.Add(location + FHexVector(1, 0, -1));
	Neighbours.Add(location + FHexVector(0, 1, -1));
	Neighbours.Add(location + FHexVector(-1, 1, 0));
	Neighbours.Add(location + FHexVector(-1, 0, 1));
	Neighbours.Add(location + FHexVector(0, -1, 1));
	Neighbours.Add(location + FHexVector(1, -1, 0));

	return Neighbours;
}
