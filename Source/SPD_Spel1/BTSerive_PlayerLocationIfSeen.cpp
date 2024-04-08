// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "BTSerive_PlayerLocationIfSeen.h"
#include "AIController.h"

UBTSerive_PlayerLocationIfSeen::UBTSerive_PlayerLocationIfSeen()
{
	NodeName = "Player Location If Seen";
}
void UBTSerive_PlayerLocationIfSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if(PlayerPawn == nullptr)
	{
		return;
	}
	if(OwnerComp.GetAIOwner() == nullptr)
	{
		return;
	}
	if(OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	}
	
}