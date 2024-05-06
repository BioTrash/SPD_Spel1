// Fill out your copyright notice in the Description page of Project Settings.


#include "RushEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"

//Hanna
ARushEnemyAIController::ARushEnemyAIController()
{
	bHasLaunched = false;
}
void ARushEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("Explode"), false);
		
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	SetFocus(PlayerPawn);
}

void ARushEnemyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn)
    {
        return; 
    }

    FVector EnemyLocation = GetPawn()->GetActorLocation();
    FVector PlayerLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("PlayerLocation"));
    float DistanceToPlayer = FVector::Distance(EnemyLocation, PlayerLocation);

    bool bIsLaunching = GetBlackboardComponent()->GetValueAsBool(TEXT("IsLaunching"));
    if (bIsLaunching)
    {
        UE_LOG(LogTemp, Warning, TEXT("Den launchar nu"));
        LaunchTowardsPlayer();
        bHasLaunched = true;
    }
    else
    {
        if (bHasLaunched) // If the enemy has launched towards the player
        {
            // Check if the enemy has reached the last seen player location
            if (DistanceToPlayer < LaunchDistanceThreshold) // Adjust threshold as necessary
            {
                // Reset behavior to normal
                GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
                bHasLaunched = false;
                // Add code here to reset any other behavior flags or values
            }
        }
        else
        {
            // Normal behavior when not launching towards player
            GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerLocation);
            if (AnticipationDelay > 0)    
            {
                AnticipationDelay -= DeltaSeconds;    
            }
            else
            {
                LaunchDistanceThreshold = FMath::RandRange(150.0f, 600.0f);
                if (DistanceToPlayer <= LaunchDistanceThreshold)
                {
                    GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), true);
                    UE_LOG(LogTemp, Warning, TEXT("Threshold: %f"), LaunchDistanceThreshold);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Den flyttar på sig?"));
                }
            }
        }
    }
}


void ARushEnemyAIController::LaunchTowardsPlayer()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		return;
	}
	FVector PlayerLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("PlayerLocation"));
	UE_LOG(LogTemp, Error, TEXT("Location: %s"), *PlayerLocation.ToString());
	FVector EnemyLocation = GetPawn()->GetActorLocation();
	FVector LaunchDirection = (PlayerLocation - EnemyLocation).GetSafeNormal();

	float LaunchSpeed = 300.f;
	FVector LaunchVelocity = LaunchDirection * LaunchSpeed;

	ACharacter* EnemyCharacter = Cast<ACharacter>(GetPawn());
	if (EnemyCharacter)
	{
		EnemyCharacter->LaunchCharacter(LaunchVelocity, true, true);
		if (EnemyLocation == PlayerLocation)
		{
		}
	}

	// Set the movement mode to "Falling" after launching towards the player
	if (EnemyCharacter)
	{
		EnemyCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}

	// GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
	UE_LOG(LogTemp, Warning, TEXT("Nu borde IsLaunching va falskt"));
}

bool ARushEnemyAIController::EnemyGrounded()
{
	ACharacter* EnemyCharacter = Cast<ACharacter>(GetPawn());
	if (EnemyCharacter)
	{
		return EnemyCharacter->GetCharacterMovement()->IsMovingOnGround();
	}
	return false;
}



