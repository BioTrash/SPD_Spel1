
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
    Enemy = Cast<AShooterBoss>(GetPawn());
    if (Enemy)
    {
        EnemyWeapon = Enemy->TriggerWeapon; //OBS! Ska bort. Enbart för att AI skapas innan Shooterenemy och därav är triggerweapon null i första beginplay().
        DirectionToPlayer = PlayerPawn->GetActorLocation() - Enemy->GetActorLocation();
        WeaponRotation = DirectionToPlayer.Rotation();

        GetBlackboardComponent()->SetValueAsFloat(TEXT("Health"), Enemy->Health);

        if (EnemyWeapon)
        {
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
                        if (LastShotTime >= ShootCooldown)
                        {
                            if (ShootingEffect && !EffectIsPlaying)
                            {
                                FVector SocketLocation = Enemy->GetStaticMeshComponent()->GetSocketLocation(TEXT("ProjectileSocket"));
                                //UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShootingEffect, EnemyWeapon->GetActorLocation()+100);
                                NiagaraSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                                   GetWorld(),
                                   ShootingEffect,
                                   EnemyWeapon->GetMuzzlePoint()->GetComponentLocation(),
                                   FRotator::ZeroRotator,
                                   FVector::OneVector
                               );
                                EffectIsPlaying = true;
                            }
                            if (NiagaraSystemComponent)
                            {
                                NiagaraSystemComponent->SetWorldLocation(Enemy->GetActorLocation() + FVector(0, 0, 100)); 
                            }
                            //const float EffectDuration = 1.5f;

                            if (LastShotTime >= ShootCooldown + 1.5f)
                            {
                                Shoot();
                                Enemy->isShooting = true;
                                LastShotTime = 0.0f;
                                EffectIsPlaying = false;
                                GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
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

void AShooterBossAIController::SpamAttack()
{
    if (LastSpamShotTime >= SpamShotCooldown && SpamAmount < 5)
    {
        Shoot();
        LastSpamShotTime = 0;
        SpamAmount++;
    }
    if (SpamAmount <= 5)
    {
        SpamAmount = 0;
    }
}

void AShooterBossAIController::Shoot()
{
    EnemyWeapon->ShootEnemyProjectile();
}

void AShooterBossAIController::InitiateEnemy()
{
    Enemy = Cast<AShooterBoss>(GetPawn());
    if (Enemy) // Ensure Enemy is not null
    {
        EnemyWeapon = Enemy->TriggerWeapon;
        if (!EnemyWeapon)
        {
            UE_LOG(LogTemp, Error, TEXT("EnemyWeapon is null"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Enemy is null"));
    }
}

void AShooterBossAIController::InitiatePlayer()
{
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}