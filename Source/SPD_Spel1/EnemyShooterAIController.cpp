// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyShooterAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterEnemy.h"
#include "Weapon.h"

void AEnemyShooterAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);

		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
	}
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	SetFocus(PlayerPawn);
	
}

//Hanna
//Sätter SetFocus på spelaren i världen, dvs att karakätren vänder på sig baserat på spelarens plats
void AEnemyShooterAIController::Tick(float DeltaSeconds)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn != nullptr)
	{
		SetFocus(PlayerPawn);
	}

	Super::Tick(DeltaSeconds);
	MoveToActor(PlayerPawn, 500);

	AShooterEnemy* Enemy = Cast<AShooterEnemy>(GetPawn());
	if (Enemy)
	{
		// Vector from shooter to the player (Louis)
		FVector DirectionToPlayer = PlayerPawn->GetActorLocation() - Enemy->GetActorLocation();

		// How much are we to rotate the weapon? (Louis)
		FRotator WeaponRotation = DirectionToPlayer.Rotation();

		// Point to the enemys weapon (Louis)
		AWeapon* EnemyWeapon = Enemy->TriggerWeapon;
		if (EnemyWeapon)
		{
			// Start rotating weapon (Louis)
			EnemyWeapon->SetActorRotation(WeaponRotation);
		}
	}
}


