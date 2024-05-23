#include "Character/RubeeWidget.h"

#include "Components/TextBlock.h"

void URubeeWidget::set_interaction_text(const FString& text)
{
	interactionText->SetText(FText::FromString(text));
}

void URubeeWidget::set_interaction_text_visibility(bool isVisible)
{
	if (isVisible)
	{
		interactionText->SetVisibility(ESlateVisibility::Visible);
	} else {
		interactionText->SetVisibility(ESlateVisibility::Hidden);
	}
}
