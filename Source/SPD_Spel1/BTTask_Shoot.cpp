// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Shoot.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTTask_ClearBlackboardValue.h"
#include "AIController.h"
#include "Shooter.h"

UBTTask_Shoot::UBTTask_Shoot()
{
	NodeName = "Shoot";
}
EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if(OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	AShooter* Character = Cast<AShooter>(OwnerComp.GetAIOwner()->GetPawn());
	if(Character == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	Character->Shoot();

	return EBTNodeResult::Succeeded;
}