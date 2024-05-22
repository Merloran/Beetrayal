#include "Equipment/EquipmentComponent.h"

#include "Equipment/Item.h"

UEquipmentComponent::UEquipmentComponent()
	: capacity(10)
	, emptyCount(10)
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UEquipmentComponent::add_item(AItem& item)
{
	bool result = false;
	if (item.get_item_size() > emptyCount)
	{
		return result;
	}

	for (TObjectPtr<AItem>& slot : slots)
	{
		if (slot == nullptr)
		{
			slot = item;
			result = true;
		}
	}

	if (result == false)
	{
		slots.Add(item);
	}

	result = true;
	emptyCount -= item.get_item_size();
	item.SetActorHiddenInGame(true);
	item.SetActorEnableCollision(false);

	return result;
}

bool UEquipmentComponent::drop_item(uint64 slotNumber)
{
	bool result = false;
	if (slotNumber >= slots.Num() || slots[slotNumber] == nullptr)
	{
		return result;
	}

	AItem &item = *slots[slotNumber];
	if (!item.can_be_dropped())
	{
		return result;
	}

	result = true;
	emptyCount += item.get_item_size();

	FVector location = GetOwner()->GetActorLocation();
	item.SetActorLocation(location);
	item.SetActorHiddenInGame(false);
	item.SetActorEnableCollision(true);

	return result;
}

void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	emptyCount = capacity;
	slots.Init(nullptr, capacity);
}
