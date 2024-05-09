#include "EnemyShooterAIController.h"

#include "EnemyWeapon.h"
#include "KismetTraceUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterEnemy.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "EnemyCommunicationManager.h"


#include "Weapon.h"
//#include "EnemyWeapon.h"

void AEnemyShooterAIController::BeginPlay()
{
    Super::BeginPlay();

    UEnemyCommunicationManager* CommunicationManager = UEnemyCommunicationManager::GetInstance();
    if (CommunicationManager)
    {
        CommunicationManager->OnPlayerLocationUpdated.AddDynamic(this, &AEnemyShooterAIController::OnPlayerLocationUpdated);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Communication manager instance is nullptr."));
    }
    InitiatePlayer();
    InitiateEnemy();
    if (AIBehavior != nullptr)
    {
        RunBehaviorTree(AIBehavior);
        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
        GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);

    }
    SetFocus(PlayerPawn);
}

void AEnemyShooterAIController::Tick(float DeltaSeconds)
{
    LastShotTime += DeltaSeconds;
    EnemyWeapon = Enemy->TriggerWeapon; //OBS! Ska bort. Enbart för att AI skapas innan Shooterenemy och därav är triggerweapon null i första beginplay().

    //GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
    if (PlayerPawn != nullptr)
    {
        SetFocus(PlayerPawn);
    }

    Super::Tick(DeltaSeconds);
    
    if (Enemy)
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy good"))

        if (EnemyWeapon)
        {
            UE_LOG(LogTemp, Warning, TEXT("Weapon Good"))

            FVector PlayerLocation = PlayerPawn->GetActorLocation();
            EnemyLocation = Enemy->GetActorLocation();

            // Calculate distance between enemy and player
            float DistanceToPlayer = FVector::Distance(EnemyLocation, PlayerLocation);

            // Check if player is within 5 meters radius
            if (DistanceToPlayer < 500.0f) // 500 units = 5 meters assuming 1 unit = 1 cm
            {
                GetBlackboardComponent()->SetValueAsBool(TEXT("InPlayerRange"), true);
                // Calculate direction away from player
                FVector DirectionAwayFromPlayer = EnemyLocation - PlayerLocation;
                DirectionAwayFromPlayer.Normalize();

                // Calculate new destination for the enemy to maintain distance
                FVector NewDestination = EnemyLocation + (DirectionAwayFromPlayer * 500.0f); // 500 units = 5 meters away
                GetBlackboardComponent()->SetValueAsVector(TEXT("BackOffLocation"), NewDestination);
                // Move enemy to new destination
               // Enemy->SetActorLocation(NewDestination);
            }
            else
            {
                GetBlackboardComponent()->SetValueAsBool(TEXT("InPlayerRange"), false);
            }
                //EnemyWeapon->SetActorRotation(WeaponRotation);
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
                    if (HitResult.GetActor() == PlayerPawn && !HitResult.GetActor()->ActorHasTag("Enemy"))
                    {
                        //Enemy updates position for PlayerLocation, LastKnownPlayerLocation, and signals it's shooting (Louis)
                        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), HitResult.GetActor()->GetActorLocation());
                        DetectPlayer((HitResult.GetActor()->GetActorLocation()));
                        //GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), HitResult.GetActor()->GetActorLocation());
                        GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), true);
                        
                        FVector LastKnownPlayerLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("LastKnownPlayerLocation"));
                        float Radius = 900.0f;
                        FVector2D RandomOffset = FMath::RandPointInCircle(Radius);
                        FVector RePositionLocation = LastKnownPlayerLocation + FVector(RandomOffset.X, RandomOffset.Y, 0.0f);

                        GetBlackboardComponent()->SetValueAsVector(TEXT("RePositionLocation"), RePositionLocation);

                        if (LastShotTime >= ShootCooldown)
                        {
                            if (ShootingEffect && !EffectIsPlaying)
                            {
                                FVector SocketLocation = Enemy->GetStaticMeshComponent()->GetSocketLocation(TEXT("ProjectileSocket"));
                                //UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShootingEffect, EnemyWeapon->GetActorLocation()+100);
                                NiagaraSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                                   GetWorld(),
                                   ShootingEffect,
                                   SocketLocation,
                                   FRotator::ZeroRotator,
                                   FVector::OneVector
                               );
                                EffectIsPlaying = true;
                            }
                            if (NiagaraSystemComponent)
                            {
                                NiagaraSystemComponent->SetWorldLocation(Enemy->GetActorLocation() + FVector(0, 0, 100)); 
                            }
                            const float EffectDuration = .9f;

                            if (LastShotTime >= ShootCooldown + EffectDuration)
                            {
                                Shoot();
                                Enemy->isShooting = true;
                                LastShotTime = 0.0f;
                                EffectIsPlaying = false;
                            }
                        }
                    }
                    //Om Ray INTE Hit Player
                        else
                        {
                            GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
                            Enemy->isShooting = false;
                        }
                    }
            }
        }
}

void AEnemyShooterAIController::DetectPlayer(const FVector& PlayerLocation)
{
    // Update player location in the player location manager
    UEnemyCommunicationManager::GetInstance()->SetPlayerLocation(PlayerLocation);
    UEnemyCommunicationManager::GetInstance()->SetIsChasing(true);
    
}


void AEnemyShooterAIController::OnPlayerLocationUpdated(const FVector& NewPlayerLocation)
{
    //UE_LOG(LogTemp, Log, TEXT("Received updated player location: %s"), *NewPlayerLocation.ToString());
    GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), NewPlayerLocation);
    UE_LOG(LogTemp, Error, TEXT("MIN PLAYER POS ÄR: %s"), *GetBlackboardComponent()->GetValueAsVector(TEXT("PlayerLocation")).ToString());

}

void AEnemyShooterAIController::Shoot()
{
    FVector SpawnLocation = EnemyWeapon->GetActorLocation(); //Byt ut till skeleton mesh senare
    SpawnLocation.X -= 100;
    FRotator SpawnRotation = WeaponRotation;
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    //Spawnar projectile och skjuter den med damage
    AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation+100, SpawnRotation, SpawnParams);
    if (Projectile)
    {
        Projectile->SetDamage(15);
    }
}

void AEnemyShooterAIController::InitiateEnemy()
{
    Enemy = Cast<AShooterEnemy>(GetPawn());
    EnemyWeapon = Enemy->TriggerWeapon;
    if (EnemyWeapon == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Weapon failed:"));

    }

    DirectionToPlayer = PlayerPawn->GetActorLocation() - Enemy->GetActorLocation();
    WeaponRotation = DirectionToPlayer.Rotation();
}

void AEnemyShooterAIController::InitiatePlayer()
{
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}