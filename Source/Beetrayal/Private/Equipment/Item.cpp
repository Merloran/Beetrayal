// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Item.h"

// Sets default values
AItem::AItem()
	: name("None")
	, size(1)
	, bCanBeDropped(true)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


const FString &AItem::get_item_name()
{
	return name;
}

int32 AItem::get_item_size() const
{
	return size;
}

bool AItem::can_be_dropped() const
{
	return bCanBeDropped;
}