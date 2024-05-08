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

	UE_LOG(LogTemp, Warning, TEXT("THIS IS THE CLOCK: %f"), LastShotTime);
	
	SetFocus(Player);
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), Player->GetActorLocation());
	
	if (Player) 
	{
		FVector PlayerLocation = Player->GetActorLocation();
		RotateHead(PlayerLocation);
	}
	if (LastShotTime >= ShootCooldown)
	{
		//Skapa projektil
		FVector SpawnLocation =ProjectileSpawn->GetComponentLocation();
		//SpawnLocation.X -= 100;
		FRotator SpawnRotation = ProjectileSpawn-> GetComponentRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		//Spawnar projectile och skjuter den med damage
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation+100, SpawnRotation, SpawnParams);
		if (Projectile)
		{
			Projectile->SetDamage(ProjectileDamage);
			UE_LOG(LogTemp, Log, TEXT("Heres the projectile: %s"), *SpawnLocation.ToString());
		}
		LastShotTime = 0.0f;
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

