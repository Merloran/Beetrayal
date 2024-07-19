// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BeeVision.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UBeeVision::NativeConstruct()
{
	Super::NativeConstruct();

	ignoreInput = false;
	currentColor = FVector4(regularColor);
	centerFadeRadius = initialCenterFadeRadius;
	currentFadeRadius = centerFadeRadius;
	update_material(currentFade, currentFadeRadius, centerFadeValue, currentColor);
}

void UBeeVision::update_material(float fade, float centerRadius, float centerFade, const FVector4 &color)
{
	UMaterialInstanceDynamic *material = background->GetDynamicMaterial();
	if (!material)
	{
		return;
	}

	FVector2D imageSize = background->GetPaintSpaceGeometry().GetAbsoluteSize();
	if (imageSize.X <= 0.0f || imageSize.Y <= 0.0f)
	{
		return;
	}

	double aspectRatio = imageSize.X / imageSize.Y;
	double invertedAspectRatio = imageSize.Y / imageSize.X;

	FVector4 c = color;
	if (centerRadius >= initialCenterFadeRadius)
	{
		c.W = 0.0f;
	}

	material->SetScalarParameterValue(FName("Fade"), fade);
	material->SetScalarParameterValue(FName("CenterFadeRadius"), centerRadius);
	material->SetScalarParameterValue(FName("CenterFadeValue"), centerFade);
	material->SetScalarParameterValue(FName("AspectRatio"), aspectRatio);
	material->SetScalarParameterValue(FName("InvertedAspectRatio"), invertedAspectRatio);
	material->SetScalarParameterValue(FName("Width"), imageSize.X);
	material->SetScalarParameterValue(FName("Height"), imageSize.Y);
	material->SetVectorParameterValue(FName("Color"), c);
}

void UBeeVision::fade(float value)
{
	UWorld *world = GetWorld();
	if (!world || ignoreInput || (centerFadeRadius == initialCenterFadeRadius && value == 1.0f))
	{
		return;
	}

	FTimerManager &timerManager = world->GetTimerManager();

	timerManager.ClearTimer(stopTimer);
	timerManager.ClearTimer(fadeTimer);

	float timePerStepMs = fadeTimeMs / float(fadeSteps);
	float intervalTimeS = timePerStepMs * 0.001f;
	float durationTimeS = (fadeTimeMs + timePerStepMs * 3.0f) * 0.001f;

	float newFadeRadius = initialCenterFadeRadius * value;
	if (newFadeRadius > centerFadeRadius) // Heal
	{
		centerFadeRadius = newFadeRadius;
		currentColor = FVector4(fadeOutColor);
		radiusStepDifference = (centerFadeRadius - currentFadeRadius) / float(fadeSteps);
		colorStepDifference = (FVector4(regularColor) - FVector4(fadeOutColor)) / float(fadeSteps);
		timerManager.SetTimer(fadeTimer, this, &UBeeVision::fade_out, intervalTimeS, true);
		timerManager.SetTimer(stopTimer, this, &UBeeVision::fade_stop, durationTimeS);
	}
	else if (newFadeRadius <= 0.0f) // Death
	{
		timePerStepMs = endTimeMs / float(fadeSteps);
		intervalTimeS = timePerStepMs * 0.001f;
		durationTimeS = (endTimeMs + timePerStepMs) * 0.001f;

		ignoreInput = true;
		centerFadeRadius = newFadeRadius;
		currentFadeRadius = centerFadeRadius;
		currentColor = FVector4(fadeInColor);
		colorStepDifference = (FVector4(regularColor) - FVector4(fadeInColor)) / float(fadeSteps);

		timerManager.SetTimer(fadeTimer, this, &UBeeVision::fade_end, intervalTimeS, true);
		timerManager.SetTimer(stopTimer, this, &UBeeVision::fade_stop, durationTimeS);
	} else { // Damage
		centerFadeRadius = newFadeRadius;
		currentFadeRadius = centerFadeRadius;
		currentColor = FVector4(fadeInColor);
		colorStepDifference = (FVector4(regularColor) - FVector4(fadeInColor)) / float(fadeSteps);

		timerManager.SetTimer(fadeTimer, this, &UBeeVision::fade_in, intervalTimeS, true);
		timerManager.SetTimer(stopTimer, this, &UBeeVision::fade_stop, durationTimeS);
	}
}

void UBeeVision::fade_in()
{
	update_material(currentFade, currentFadeRadius, centerFadeValue, currentColor);

	currentColor += colorStepDifference;
	currentColor.W = 1.0f;
}

void UBeeVision::fade_out()
{
	update_material(currentFade, currentFadeRadius, centerFadeValue, currentColor);

	if (currentFadeRadius < centerFadeRadius)
	{
		currentFadeRadius += radiusStepDifference;
	}

	if (currentColor != FVector4(regularColor))
	{
		currentColor += colorStepDifference;
		currentColor.W = 1.0f;
	}
}

void UBeeVision::fade_end()
{
	update_material(currentFade, currentFadeRadius, centerFadeValue, currentColor);
	currentColor += colorStepDifference;
	currentColor.W = 1.0f;

	currentFade -= endFadeSpeed;

	FLinearColor color = text->GetColorAndOpacity().GetSpecifiedColor();
	color.A += textFadeSpeed;
	text->SetColorAndOpacity(color);
}

void UBeeVision::fade_stop()
{
	UWorld *world = GetWorld();
	if (!world)
	{
		return;
	}

	FTimerManager &timerManager = world->GetTimerManager();

	timerManager.ClearTimer(fadeTimer);

	if (ignoreInput)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}
