// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Shooter.h"

void AShooterAIController::BeginPlay()
{
	Super::BeginPlay();
	if(AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);

		APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		
		
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
		
	}														
}
void AShooterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

bool AShooterAIController::isDead() const
{
	AShooter* ControlledCharacter = Cast<AShooter>(GetPawn());
	if (ControlledCharacter != nullptr)
	{
		return ControlledCharacter->IsDead();
	}
	return true;
}



