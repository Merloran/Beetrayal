#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_DistanceCheck.generated.h"

UCLASS()
class BEETRAYAL_API UBTDecorator_DistanceCheck : public UBTDecorator_Blackboard
{
	GENERATED_BODY()

public:
	UBTDecorator_DistanceCheck();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, Category = "Distance")
	float acceptableDistance;
};
