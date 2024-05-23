#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RubeeWidget.generated.h"

class UTextBlock;
UCLASS()
class BEETRAYAL_API URubeeWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> interactionText;

public:
	void set_interaction_text(const FString& text);
	void set_interaction_text_visibility(bool isVisible);
};
