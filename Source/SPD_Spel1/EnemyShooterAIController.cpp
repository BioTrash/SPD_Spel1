// Fill out your copyright notice in the Description page of Project Settings.

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
    if (AIBehavior != nullptr)
    {
        RunBehaviorTree(AIBehavior);

        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
        GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);

    }
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    SetFocus(PlayerPawn);
}

void AEnemyShooterAIController::Tick(float DeltaSeconds)
{
    LastShotTime += DeltaSeconds;
    //GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);

    
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn != nullptr)
    {
        SetFocus(PlayerPawn);
    }

    Super::Tick(DeltaSeconds);

    AShooterEnemy* Enemy = Cast<AShooterEnemy>(GetPawn());
    EnemyLocation = Enemy->GetActorLocation();
    
    if (Enemy)
    {
        FVector DirectionToPlayer = PlayerPawn->GetActorLocation() - Enemy->GetActorLocation();
        FRotator WeaponRotation = DirectionToPlayer.Rotation();

        AEnemyWeapon* EnemyWeapon = Enemy->TriggerWeapon;
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
                    if (HitResult.GetActor() == PlayerPawn && !HitResult.GetActor()->ActorHasTag("Enemy"))
                    {
                        //Enemy updates position for PlayerLocation, LastKnownPlayerLocation, and signals it's shooting (Louis)
                        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), HitResult.GetActor()->GetActorLocation());
                        DetectPlayer((HitResult.GetActor()->GetActorLocation()));
                        GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), HitResult.GetActor()->GetActorLocation());
                        GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), true);
                        Enemy->isShooting = true;
                        
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
                                NiagaraSystemComponent->SetWorldLocation(Enemy->GetActorLocation() + FVector(0, 0, 100)); // Adjust the offset as needed
                            }
                            const float EffectDuration = .9f;

                            if (LastShotTime >= ShootCooldown + EffectDuration)
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
                                    UE_LOG(LogTemp, Log, TEXT("Heres the projectile: %s"), *SpawnLocation.ToString());

                                }
                                OnEnemyShoot();
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
}


void AEnemyShooterAIController::OnPlayerLocationUpdated(const FVector& NewPlayerLocation)
{
    UE_LOG(LogTemp, Log, TEXT("Received updated player location: %s"), *NewPlayerLocation.ToString());
    GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), NewPlayerLocation);
}
