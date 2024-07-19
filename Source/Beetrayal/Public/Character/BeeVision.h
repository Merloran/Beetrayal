// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BeeVision.generated.h"

class UTextBlock;
class UImage;
UCLASS()
class BEETRAYAL_API UBeeVision : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> background;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> text;

	UPROPERTY(EditAnywhere, Category = "Fade")
	FColor fadeInColor;

	UPROPERTY(EditAnywhere, Category = "Fade")
	FColor fadeOutColor;

	UPROPERTY(EditAnywhere, Category = "Fade")
	FColor regularColor;

	UPROPERTY(EditAnywhere, Category = "Fade")
	float textFadeSpeed;

	UPROPERTY(EditAnywhere, Category = "Fade")
	float endFadeSpeed;

	UPROPERTY(EditAnywhere, Category = "Fade")
	float endTimeMs;

	UPROPERTY(EditAnywhere, Category = "Fade")
	float fadeTimeMs;

	UPROPERTY(EditAnywhere, Category = "Fade")
	int32 fadeSteps;

	UPROPERTY(EditAnywhere, Category = "Fade")
	float centerFadeValue;

	UPROPERTY(EditAnywhere, Category = "Fade")
	float initialCenterFadeRadius;

	UPROPERTY(EditAnywhere, Category = "Fade")
	float currentFade;

	UPROPERTY(BlueprintReadOnly, Category = "Fade")
	FTimerHandle fadeTimer;

	UPROPERTY(BlueprintReadOnly, Category = "Fade")
	FTimerHandle stopTimer;
private:
	FVector4 currentColor;

	/** current replies to displayed fade during animation */
	float currentFadeRadius, centerFadeRadius;

	bool ignoreInput;

	FVector4 colorStepDifference;

	float radiusStepDifference;

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void update_material(float fade, float centerRadius, float centerFade, const FVector4 &color);

	UFUNCTION(BlueprintCallable)
	void fade(float value);

private:
	void fade_in();
	void fade_out();
	void fade_end();
	void fade_stop();
};

inline FColor operator+(const FColor &color1, const FColor &color2)
{
	return FColor(color1.R + color2.R, color1.G + color2.G, color1.B + color2.B, color1.A + color2.A);
}

inline FColor operator-(const FColor &color1, const FColor &color2)
{
	return FColor(color1.R - color2.R, color1.G - color2.G, color1.B - color2.B, color1.A - color2.A);
}

inline FColor operator/(const FColor &color1, const float scalar)
{
	return FColor(color1.R / scalar, color1.G / scalar, color1.B / scalar, color1.A / scalar);
}