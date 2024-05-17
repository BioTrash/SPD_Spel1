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
//Run behavior tree och körs när spelet startar
void ARushEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (AIBehavior)
	{
		RunBehaviorTree(AIBehavior);
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
	}
	//Fokus på spelaren
	SetFocus(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}
//Kallas varje frame, kallar på andra metoder
void ARushEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdatePlayerLocation();
	LaunchLogic(DeltaSeconds);
}
//Uppdaterar spelarens location i blackboard
void ARushEnemyAIController::UpdatePlayerLocation()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
		return;

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerLocation);
}
//Hanterar launchlogiken baserat på distansen på spelaren
void ARushEnemyAIController::LaunchLogic(float DeltaSeconds)
{
	FVector EnemyLocation = GetPawn()->GetActorLocation();
	FVector PlayerLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("PlayerLocation"));

	float DistanceToPlayer = FVector::Distance(EnemyLocation, PlayerLocation);
	bool bIsLaunching = GetBlackboardComponent()->GetValueAsBool(TEXT("IsLaunching"));

	if (bIsLaunching && bHasLaunched)
	{
		bHasLaunched = false;
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
	}

	if (bIsLaunching && !bHasLaunched)
	{
		LaunchTowardsPlayer();
		bHasLaunched = true;
	}
	else if (!bIsLaunching && bHasLaunched)
	{
		bHasLaunched = false;
	}
	else
	{
		AnticipationLaunch(5, EnemyLocation, PlayerLocation, DistanceToPlayer);
	}
}
//Anticipatar launchen mot spelaren
void ARushEnemyAIController::AnticipationLaunch(float DeltaSeconds, const FVector& EnemyLocation, const FVector& PlayerLocation, float DistanceToPlayer)
{
	if (AnticipationDelay > 0)
	{
		AnticipationDelay -= DeltaSeconds;
	}
	else
	{
		LaunchDistanceThreshold = FMath::RandRange(150.0f, 600.0f);
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetPawn()); 
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, EnemyLocation, PlayerLocation, ECC_Visibility, CollisionParams);
		if (!bHit && (DistanceToPlayer <= LaunchDistanceThreshold))
		{
			GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), true);
			LaunchTowardsPlayer(); 
		}
	}
}
//Fienden launchar sig mot spelaren
void ARushEnemyAIController::LaunchTowardsPlayer()
{
	FVector PlayerLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("PlayerLocation"));
	FVector EnemyLocation = GetPawn()->GetActorLocation();
	FVector LaunchDirection = (PlayerLocation - EnemyLocation).GetSafeNormal();

	float LaunchSpeed = 1000.f;
	if (ACharacter* EnemyCharacter = Cast<ACharacter>(GetPawn()))
	{
		float JumpHeight = 600.f;
		EnemyCharacter->LaunchCharacter(LaunchDirection * LaunchSpeed + FVector::UpVector * JumpHeight, true, true);
	}

	//Sätter spelarens location i blackboard och hanterar explosion
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerLocation);
	if (!GetWorldTimerManager().IsTimerActive(ExplodeTimerHandle))
	{
		float TimeToReachPlayer = (PlayerLocation - EnemyLocation).Size() / LaunchSpeed;
		float DelayBeforeExplode = FMath::Max(TimeToReachPlayer * 0.2f, 0.4f);
		GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &ARushEnemyAIController::ExplodeAfterLaunch, DelayBeforeExplode, false);
	}

	bHasLaunched = false;
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
}
//Kallar på launching staten så att den exploderar efter den launchats mot spelaren
void ARushEnemyAIController::ExplodeAfterLaunch()
{
	ARushEnemyAI* RushEnemy = Cast<ARushEnemyAI>(GetPawn());
	if (RushEnemy && !RushEnemy->bHasExploded)
	{
		RushEnemy->Explode(ExplosionDamage, true);
		FVector ExplosionLocation = RushEnemy->GetActorLocation();
		if (ExplosionEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, ExplosionLocation, FRotator::ZeroRotator, FVector::OneVector, true, true);
		}
	}
}