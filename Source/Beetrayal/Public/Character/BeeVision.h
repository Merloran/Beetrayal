// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BeeVision.generated.h"

class UImage;
UCLASS()
class BEETRAYAL_API UBeeVision : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> background;

public:
	UFUNCTION(BlueprintCallable)
	void update_material(float fade, float centerFadeRadius, float centerFadeValue, const FVector4 &color);
};
