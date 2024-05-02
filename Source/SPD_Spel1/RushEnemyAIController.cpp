// Fill out your copyright notice in the Description page of Project Settings.


#include "RushEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"

//Hanna
void ARushEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	SetFocus(PlayerPawn);
}
void ARushEnemyAIController::LaunchTowardsPlayer()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		return;
	}

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector EnemyLocation = GetPawn()->GetActorLocation();
	FVector LaunchDirection = (PlayerLocation - EnemyLocation).GetSafeNormal();
	
	float LaunchSpeed = 600.f; 
	FVector LaunchVelocity = LaunchDirection * LaunchSpeed;
    
	ACharacter* EnemyCharacter = Cast<ACharacter>(GetPawn());
	if (EnemyCharacter)
	{
		EnemyCharacter->LaunchCharacter(LaunchVelocity, true, true);
	}
	
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
}

void ARushEnemyAIController::Explode(float Damage)
{
	ACharacter* PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		float DistanceToPlayer = FVector::Distance(GetPawn()->GetActorLocation(), PlayerCharacter->GetActorLocation());
		float DistanceMultiplier = FMath::Clamp(1.0f - (DistanceToPlayer / DamageRadius), 0.0f, 1.0f);
		float ActualDamage = PlayerCharacter->TakeDamage(Damage * DistanceMultiplier, FDamageEvent(), GetPawn()->GetController(), this);

		UE_LOG(LogTemp, Warning, TEXT("Damage done: %f"), ActualDamage);
	}
	UE_LOG(LogTemp, Warning, TEXT("Enemy did damage!"));
	DrawDebugSphere(GetWorld(), GetPawn()->GetActorLocation(), DamageRadius, 32, FColor::Red, false, 5.0f);
}

//Hanna
//Sätter SetFocus på spelaren i världen, dvs att karakätren vänder på sig baserat på spelarens plats
void ARushEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		return; 
	}

	FVector EnemyLocation = GetPawn()->GetActorLocation();
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	float DistanceToPlayer = FVector::Distance(EnemyLocation, PlayerLocation);

	bool bIsLaunching = GetBlackboardComponent()->GetValueAsBool(TEXT("IsLaunching"));
	UE_LOG(LogTemp, Error, TEXT("BOOL LAUNCHING : "), bIsLaunching);
	if (bIsLaunching)
	{
		LaunchTowardsPlayer(); 
	}
	else
	{
		if (DistanceToPlayer <= LaunchDistanceThreshold)
		{
			GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), true);
			UE_LOG(LogTemp, Warning, TEXT("Launchar du nä?"));
		}
		else
		{
			MoveToActor(PlayerPawn); // Adjusted to use LaunchDistanceThreshold
			UE_LOG(LogTemp, Warning, TEXT("Den flyttar på sig?"));
		}
	}
	}
void ARushEnemyAIController::KillEnemy()
{
	UE_LOG(LogTemp, Warning, TEXT("ENEMY SHOULD DIE"));
	Destroy();
}


