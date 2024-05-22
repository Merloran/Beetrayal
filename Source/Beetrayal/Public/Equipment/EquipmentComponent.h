// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


class AItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEETRAYAL_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	int32 capacity;

	int32 emptyCount;

private:
	TArray<TObjectPtr<AItem>> slots;

public:
	UEquipmentComponent();

	bool add_item(AItem &item);

	bool drop_item(uint64 slotNumber);

protected:
	virtual void BeginPlay() override;
};