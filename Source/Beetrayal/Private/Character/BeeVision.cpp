// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BeeVision.h"

#include "Components/Image.h"

void UBeeVision::update_material(float fade, float centerFadeRadius, float centerFadeValue, const FVector4& color)
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

	material->SetScalarParameterValue(FName("Fade"), fade);
	material->SetScalarParameterValue(FName("CenterFadeRadius"), centerFadeRadius);
	material->SetScalarParameterValue(FName("CenterFadeValue"), centerFadeValue);
	material->SetScalarParameterValue(FName("AspectRatio"), aspectRatio);
	material->SetScalarParameterValue(FName("InvertedAspectRatio"), invertedAspectRatio);
	material->SetScalarParameterValue(FName("Width"), imageSize.X);
	material->SetScalarParameterValue(FName("Height"), imageSize.Y);
	material->SetVectorParameterValue(FName("Color"), color);
}
