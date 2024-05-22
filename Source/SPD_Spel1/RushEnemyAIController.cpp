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
	//Sätter bHasLaunched till false
	bHasLaunched = false;
}
//Run behavior tree och körs när spelet startar
void ARushEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	//Kontroller om AIBehavior är satt då själva behavior tree
	if (AIBehavior)
	{
		//Kör behavior-tree som finns i AIBehavior
		RunBehaviorTree(AIBehavior);
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
	}
	//Fokus på spelarens pawn, AI tittar på spelaren
	SetFocus(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}
//Kallas varje frame, kallar på andra metoder
void ARushEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//Uppdaterar spelarens position i blackboard
	UpdatePlayerLocation();
	//Launch-logiken baserat på avståndet till spelaren
	LaunchLogic(DeltaSeconds);
}
//Uppdaterar spelarens location i blackboard
void ARushEnemyAIController::UpdatePlayerLocation()
{
	//Hämtar spelarens pawn
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	//Kollar om PlayerPawn är giltig 
	if (PlayerPawn)
	{
		//Hämtar spelarens position
		FVector PlayerLocation = PlayerPawn->GetActorLocation();
		//Sätter sedan positionen i blackboard
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerLocation);
	}
}
//Hanterar launchlogiken baserat på distansen på spelaren
void ARushEnemyAIController::LaunchLogic(float DeltaSeconds)
{
	//Hämtar fiendens position
	FVector EnemyLocation = GetPawn()->GetActorLocation();
	//Hämtar spelarens position från blackboard
	FVector PlayerLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("PlayerLocation"));

	//Beräknar avståndet mellan fienden och spelaren
	float DistanceToPlayer = FVector::Distance(EnemyLocation, PlayerLocation);
	//Hämtar om fienden launchas mot spelaren
	bool bIsLaunching = GetBlackboardComponent()->GetValueAsBool(TEXT("IsLaunching"));

	//Kollar om fienden launchas och redan har launchar
	if (bIsLaunching)
	{
		if (bHasLaunched)
		{
			bHasLaunched = false;
			GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
		}
		else
		{
			LaunchTowardsPlayer();
			bHasLaunched = true;
		}
	}
	else
	{
		AnticipationLaunch(DeltaSeconds, EnemyLocation, PlayerLocation, DistanceToPlayer);
	}
}
//Anticipatar launchen mot spelaren
void ARushEnemyAIController::AnticipationLaunch(float DeltaSeconds, const FVector& EnemyLocation, const FVector& PlayerLocation, float DistanceToPlayer)
{
	//Kollar om anticipation delay är större än 0
	if (AnticipationDelay > 0)
	{
		//Minskar delay med deltaseconds
		AnticipationDelay -= DeltaSeconds;
	}
	else
	{
		//Sätter ett random värde mellan 150-600 för launchavståndet
		LaunchDistanceThreshold = FMath::RandRange(150.0f, 600.0f);
		//Initerar hitresult och collisionparams
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		//Ignorerar själva fienden 
		CollisionParams.AddIgnoredActor(GetPawn());
		//Kollar med linetracing siktet mot spelaren
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, EnemyLocation, PlayerLocation, ECC_Visibility, CollisionParams);
		//Kontrollerar om det inte fanns någon träff och om avstånden är inom launch thresholden
		if (!bHit && (DistanceToPlayer <= LaunchDistanceThreshold))
		{
			//Sötter IsLaunching till true
			GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), true);
			LaunchTowardsPlayer(); 
		}
	}
}
//Fienden launchar sig mot spelaren
void ARushEnemyAIController::LaunchTowardsPlayer()
{
	//Hämtar spelarens position
	FVector PlayerLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("PlayerLocation"));
	//Hämtar fiendens position
	FVector EnemyLocation = GetPawn()->GetActorLocation();
	//Beräknar launch riktningen mot spelaren
	FVector LaunchDirection = (PlayerLocation - EnemyLocation).GetSafeNormal();

	//Sätter launch hastighet
	float LaunchSpeed = 1000.f;
	//Kontrollerar ifall fienden är en karaktär
	if (ACharacter* EnemyCharacter = Cast<ACharacter>(GetPawn()))
	{
		//Sätter hopphöjden
		float JumpHeight = 600.f;
		//Launchar karaktären mot spelaren
		EnemyCharacter->LaunchCharacter(LaunchDirection * LaunchSpeed + FVector::UpVector * JumpHeight, true, true);
	}

	//Sätter spelarens location i blackboard och hanterar explosion
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerLocation);

	//Kontrollerar om explode timern inte redan är igång/aktiv
	if (!GetWorldTimerManager().IsTimerActive(ExplodeTimerHandle))
	{
		//Beröknar ut tiden som det tar att nå spelaren
		float TimeToReachPlayer = (PlayerLocation - EnemyLocation).Size() / LaunchSpeed;
		//Beräknar färdröjning innan explosion
		float DelayBeforeExplode = FMath::Max(TimeToReachPlayer * 0.2f, 0.4f);
		//Timer för att anropa ExplodeAfterLaunch funktionen efter fördröjning
		GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &ARushEnemyAIController::ExplodeAfterLaunch, DelayBeforeExplode, false);
	}
	//Sätter IsLaunching till false
	bHasLaunched = false;
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsLaunching"), false);
}
//Kallar på launching staten så att den exploderar efter den launchats mot spelaren
void ARushEnemyAIController::ExplodeAfterLaunch()
{
	//Hämtar fiendeobjektet och dens klass
	ARushEnemyAI* RushEnemy = Cast<ARushEnemyAI>(GetPawn());
	//Kollar om RushEnemy är giltig och om den inte har exploderat än
	if (RushEnemy && !RushEnemy->bHasExploded)
	{
		//Utför explosionen med explosiondamage
		RushEnemy->Explode(ExplosionDamage, true);
		//Hämtar explosionens position
		FVector ExplosionLocation = RushEnemy->GetActorLocation();
		//Explosioneffekten
		if (ExplosionEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, ExplosionLocation, FRotator::ZeroRotator, FVector::OneVector, true, true);
		}
	}
}