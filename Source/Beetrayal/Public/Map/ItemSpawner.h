// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawner.generated.h"

class AItem;

USTRUCT()
struct FItemPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FInt32Vector2 chance;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AItem> item;

};

UCLASS()
class BEETRAYAL_API AItemSpawner : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TArray<FItemPair> items;

public:
	AItemSpawner();

	UFUNCTION(BlueprintCallable)
	void spawn_item();

protected:
	virtual void BeginPlay() override;
};
