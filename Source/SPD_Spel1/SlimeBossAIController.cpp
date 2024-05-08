// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeBossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void ASlimeBossAIController::BeginPlay()
{
	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);

		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"),true);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseTwo"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseThree"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("ShouldSpawn"), false);
	}
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	SetFocus(PlayerPawn);
}
void ASlimeBossAIController::Tick(float DeltaSeconds)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(TurretMesh->GetComponentLocation(), TargetLocation);
	LookAtRotation.Pitch = 0;
	LookAtRotation.Roll = 0;
	LookAtRotation.Yaw += -90.f; 
	HeadMesh->SetWorldRotation(LookAtRotation);
}

void ASlimeBossAIController::RotateHead(FVector TargetLocation)
{
	
}
void ASlimeBossAIController::FireCooldown()
{
	UE_LOG(LogTemp, Warning, TEXT("HEJHEJ"));
}


