
#include "ShooterBossAIController.h"
#include "EnemyWeapon.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterEnemy.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "EnemyCommunicationManager.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "EnemyWeapon.h"

void AShooterBossAIController::BeginPlay()
{
    Super::BeginPlay();
    
    {
        UE_LOG(LogTemp, Error, TEXT("Communication manager instance is nullptr."));
    }
    InitiatePlayer();
    InitiateEnemy();
    if (AIBehavior != nullptr)
    {
        RunBehaviorTree(AIBehavior);
    }
    SetFocus(PlayerPawn);
}

void AShooterBossAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
    LastShotTime += DeltaSeconds;
    LastSpamShotTime += DeltaSeconds;
    if (PlayerPawn != nullptr)
    {
        SetFocus(PlayerPawn);
    }
    Enemy = Cast<AShooterEnemy>(GetPawn());
    if (Enemy)
    {
        EnemyWeapon = Enemy->TriggerWeapon; //OBS! Ska bort. Enbart för att AI skapas innan Shooterenemy och därav är triggerweapon null i första beginplay().
        DirectionToPlayer = PlayerPawn->GetActorLocation() - Enemy->GetActorLocation();
        WeaponRotation = DirectionToPlayer.Rotation();

        GetBlackboardComponent()->SetValueAsFloat(TEXT("Health"), Enemy->Health);

        if (EnemyWeapon)
        {
            SpamAttack();

            FVector PlayerLocation = PlayerPawn->GetActorLocation();
            EnemyLocation = Enemy->GetActorLocation();
            float DistanceToPlayer = FVector::Distance(EnemyLocation, PlayerLocation);
            
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
                            /*
                            if (LastShotTime >= ShootCooldown + EffectDuration)
                            {
                                UE_LOG(LogTemp, Warning, TEXT("SHOOT!"));
                                Shoot();
                                Enemy->isShooting = true;
                                LastShotTime = 0.0f;
                                EffectIsPlaying = false;
                            }*/
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


void AShooterBossAIController::Shoot()
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

void AShooterBossAIController::SpamAttack()
{
    if (LastSpamShotTime >= SpamShotCooldown && SpamAmount < 5)
    {
        Shoot();
        LastSpamShotTime = 0;
        SpamAmount++;
    }
}

void AShooterBossAIController::InitiateEnemy()
{
    Enemy = Cast<AShooterEnemy>(GetPawn());
    //EnemyWeapon = Enemy->TriggerWeapon;
}

void AShooterBossAIController::InitiatePlayer()
{
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}