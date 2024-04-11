// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyShooterAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

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


void AEnemyShooterAIController::Tick(float DeltaSeconds)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn != nullptr)
	{
		SetFocus(PlayerPawn);
	}

<<<<<<< HEAD
    Super::Tick(DeltaSeconds);
    MoveToActor(PlayerPawn, 500);

    AShooterEnemy* Enemy = Cast<AShooterEnemy>(GetPawn());
    if (Enemy)
    {
        FVector DirectionToPlayer = PlayerPawn->GetActorLocation() - Enemy->GetActorLocation();
        FRotator WeaponRotation = DirectionToPlayer.Rotation();

        AWeapon* EnemyWeapon = Enemy->TriggerWeapon;
        if (EnemyWeapon)
        {
            EnemyWeapon->SetActorRotation(WeaponRotation);

            // Vectors where trace is happening (Louis)
            FVector StartTrace = EnemyWeapon->GetActorLocation();
            FVector EndTrace = PlayerPawn->GetActorLocation();

            //Params for linetrace (Louis)
            FHitResult HitResult;
            FCollisionQueryParams CollisionParams;
            CollisionParams.AddIgnoredActor(Enemy);

            // Perform a line trace to check if there's a clear line of sight to the player (Louis)
            if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_GameTraceChannel1, CollisionParams))
            {
                // If the ray hits the player, shoot (Louis)
                if (HitResult.GetActor() == PlayerPawn)
                {
                    DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 0.1f, 0, 2);
                    EnemyWeapon->PullTrigger();
                    EnemyWeapon->PullTrigger();
                }
            }

            // Visualize the line trace
            
        }
    }
=======
	Super::Tick(DeltaSeconds);
	MoveToActor(PlayerPawn, 500);
>>>>>>> main
}


