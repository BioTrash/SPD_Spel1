// Fill out your copyright notice in the Description page of Project Settings.


#include "RushEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "RushEnemyAI.h"
#include "NiagaraFunctionLibrary.h"

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

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerLocation);
	FVector EnemyLocation = GetPawn()->GetActorLocation();
	float DistanceToPlayer = FVector::Distance(EnemyLocation, PlayerLocation);

	bool bIsLaunching = GetBlackboardComponent()->GetValueAsBool(TEXT("IsLaunching"));
	if (bIsLaunching && !bHasLaunched)
	{
		LaunchTowardsPlayer();
		bHasLaunched = true;
	}
	else if (!bIsLaunching && bHasLaunched)
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerLocation);
		bHasLaunched = false;
	}
	else
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerLocation);

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

	FVector PlayerLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("LastKnownPlayerLocation"));
	FVector EnemyLocation = GetPawn()->GetActorLocation();
	FVector LaunchDirection = (PlayerLocation - EnemyLocation).GetSafeNormal();

	float LaunchSpeed = 1200.f;

	ACharacter* EnemyCharacter = Cast<ACharacter>(GetPawn());
	if (EnemyCharacter)
	{
		EnemyCharacter->LaunchCharacter(LaunchDirection * LaunchSpeed, true, true);
	}

	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerLocation);
	if (!GetWorldTimerManager().IsTimerActive(ExplodeTimerHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit kom jag :)"));
		float TimeToReachPlayer = (PlayerLocation - EnemyLocation).Size() / LaunchSpeed;
		float DelayBeforeExplode = FMath::Max(TimeToReachPlayer * 0.2f, 0.4f);
		GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &ARushEnemyAIController::ExplodeAfterLaunch, DelayBeforeExplode, false);
	}

	bHasLaunched = false;
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
	UE_LOG(LogTemp, Warning, TEXT("Launching towards the player"));
}
void ARushEnemyAIController::ExplodeAfterLaunch()
{
	UE_LOG(LogTemp, Warning, TEXT("Det går bra här"));
	ARushEnemyAI* RushEnemy = Cast<ARushEnemyAI>(GetPawn());
	if (RushEnemy && !RushEnemy->bHasExploded)
	{
		RushEnemy->Explode(ExplosionDamage, true);
		FVector ExplosionLocation = RushEnemy->GetActorLocation();
		DrawDebugSphere(GetWorld(), ExplosionLocation, ExplosionDamage, 12, FColor::Red, false, 1.0f);
		if (ExplosionEffect)
		{
			UE_LOG(LogTemp, Warning, TEXT("Explosion?"));
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, ExplosionLocation, FRotator::ZeroRotator, FVector::OneVector, true, true);
		}
		}
}





