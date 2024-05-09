// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeBossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.h"
#include "SlimeBossAI.h"
#include "Projectile.h"

void ASlimeBossAIController::BeginPlay()
{
	Super::BeginPlay();
	SetPlayer();
	if (Boss)
	{
		PawnMesh = Boss->FindComponentByClass<UStaticMeshComponent>();
		ProjectileSpawn = Boss->FindComponentByClass<USceneComponent>();
		if (ProjectileSpawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("SPAWN FOUND" ));
		}
	}	
	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);
		
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), Player->GetActorLocation());
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"),true);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseTwo"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseThree"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("ShouldSpawn"), false);
	}
	SetFocus(Player);
}
void ASlimeBossAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	LastShotTime += DeltaSeconds;

	SetFocus(Player);
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), Player->GetActorLocation());

	if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		RotateHead(PlayerLocation);
	}
	UpdateBossPhase();
	if (LastShotTime >= ShootCooldown)
	{
		Shoot();
		LastShotTime = 0.0f;
	}
}
void ASlimeBossAIController::Shoot()
{
	FVector SpawnLocation = ProjectileSpawn->GetComponentLocation();
	FRotator SpawnRotation = ProjectileSpawn->GetComponentRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (Projectile)
	{
		Projectile->SetDamage(ProjectileDamage);
	}
}
void ASlimeBossAIController::RotateHead(FVector TargetLocation)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PawnMesh->GetComponentLocation(), TargetLocation);
	LookAtRotation.Pitch = 0;
	LookAtRotation.Roll = 0;
	LookAtRotation.Yaw += -90.f; 
	PawnMesh->SetWorldRotation(LookAtRotation);
}
void ASlimeBossAIController::FireCooldown()
{
	UE_LOG(LogTemp, Warning, TEXT("HEJHEJ"));
}

void ASlimeBossAIController::SetPlayer()
{
	if (UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}
	if (Player == nullptr)
	{
		//Shrek ;)
	}

	if (Cast<ASlimeBossAI>(GetPawn()))
	{
		Boss = Cast<ASlimeBossAI>(GetPawn());
	}

	if (Boss == nullptr)
	{
	}
}
void ASlimeBossAIController::UpdateBossPhase()
{
	ASlimeBossAI* SlimeBoss = Cast<ASlimeBossAI>(GetPawn());
	if (SlimeBoss)
	{
		float Health = SlimeBoss->GetHealth();
		if (Health >= 150)
		{
			UE_LOG(LogTemp, Warning, TEXT("nu börjar phase one"));
			BossPhaseOne();
		}
		else if (Health >= 100)
		{
			UE_LOG(LogTemp, Warning, TEXT("nu börjar phase two"));
			BossPhaseTwo();
		}
		else if (Health >= 50)
		{
			UE_LOG(LogTemp, Warning, TEXT("nu börjar phase three"));
			BossPhaseThree();
		}
	}
}
void ASlimeBossAIController::BossPhaseOne()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseOne"),true);
  	//Bossen ska fokusera på att skjuta på spelaren
 	//Sätta phasetwo and three till falskt
}
void ASlimeBossAIController::BossPhaseTwo()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseTwo"), true);
	//Spawna in RushEnemy fiender 
	//Sätta phaseone and three till falskt
}
void ASlimeBossAIController::BossPhaseThree()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("PhaseThree"), true);
	//Göra en slam här där den spawnar in mer fiender pluys 
	//Sätta phaseone and two till falskt
}
