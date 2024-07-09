#include "AI/BTDecorator_DistanceCheck.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_DistanceCheck::UBTDecorator_DistanceCheck()
	: acceptableDistance(0.0f)
{
	NodeName = "Check Distance to Point";
}

bool UBTDecorator_DistanceCheck::CalculateRawConditionValue(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) const
{
	UBlackboardComponent *blackboard = OwnerComp.GetBlackboardComponent();
	if (!blackboard)
	{
		return false;
	}

	FVector targetLocation = blackboard->GetValueAsVector(BlackboardKey.SelectedKeyName);
	FVector selfLocation = OwnerComp.GetOwner()->GetActorLocation();

	float distanceSquared = FVector::DistSquared(targetLocation, selfLocation);
	return distanceSquared <= acceptableDistance * acceptableDistance;
}
