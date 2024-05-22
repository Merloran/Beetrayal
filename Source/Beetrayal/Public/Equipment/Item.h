// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class BEETRAYAL_API AItem : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Item")
	FString name;

	UPROPERTY(EditAnywhere, Category = "Item")
	int32 size;

	UPROPERTY(EditAnywhere, Category = "Item")
	bool bCanBeDropped;

public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

	const FString &get_item_name();
	int32 get_item_size() const;
	bool can_be_dropped() const;

protected:
	virtual void BeginPlay() override;

};
