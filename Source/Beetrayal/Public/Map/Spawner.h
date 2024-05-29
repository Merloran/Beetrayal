// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS()
class BEETRAYAL_API ASpawner : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> Enemies;

public:	
	ASpawner();

protected:
	virtual void BeginPlay() override;
};
