
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
    // Set the initial focus to the player pawn
    if (PlayerPawn != nullptr)
    {
        SetFocus(PlayerPawn);
    }
    ShootEffect->OnSystemFinished.AddDynamic(this, &ASlimeBossAIController::OnNiagaraSystemFinished);
}

void AShooterBossAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (PlayerPawn != nullptr)
    {
        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
        GetBlackboardComponent()->SetValueAsFloat(TEXT("Health"), Enemy ? Enemy->Health : 0.0f);
        
        LastShotTime += DeltaSeconds;
        LastSpamShotTime += DeltaSeconds;

        Enemy = Cast<AShooterBoss>(GetPawn());
        if (Enemy)
        {
            EnemyWeapon = Enemy->TriggerWeapon; // Ensure TriggerWeapon is set
            DirectionToPlayer = PlayerPawn->GetActorLocation() - Enemy->GetActorLocation();
            WeaponRotation = DirectionToPlayer.Rotation();

            if (EnemyWeapon)
            {
                FVector PlayerLocation = PlayerPawn->GetActorLocation();
                EnemyLocation = Enemy->GetActorLocation();
                float DistanceToPlayer = FVector::Distance(EnemyLocation, PlayerLocation);
            
                if (DistanceToPlayer < 900.0f)
                {
                    //Logik bakom ny position att röra sig mot om fienden befinner sig för nära
                    GetBlackboardComponent()->SetValueAsBool(TEXT("InPlayerRange"), true);
                    FVector DirectionAwayFromPlayer = EnemyLocation - PlayerLocation;
                    DirectionAwayFromPlayer.Normalize();

                    FVector NewDestination = EnemyLocation + (DirectionAwayFromPlayer * 500.0f);
                    GetBlackboardComponent()->SetValueAsVector(TEXT("BackOffLocation"), NewDestination);

                    if (DistanceToPlayer < 500.0f)
                    {
                        GetBlackboardComponent()->SetValueAsBool(TEXT("RageShot"), true);
                        SpamAttack();
                    }
                }
                
                FVector StartTrace = EnemyWeapon->GetActorLocation();
                FVector EndTrace = PlayerPawn->GetActorLocation();

                FHitResult HitResult;
                FCollisionQueryParams CollisionParams;
                CollisionParams.AddIgnoredActor(Enemy);

                if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_GameTraceChannel1, CollisionParams))
                {
                    if (HitResult.GetActor() == PlayerPawn && !HitResult.GetActor()->ActorHasTag("Enemy"))
                    {
                        if (LastShotTime >= ShootCooldown)
                        {
                            if (ShootingEffect && !EffectIsPlaying)
                            {
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
                                NiagaraSystemComponent->SetWorldLocation(EnemyWeapon->GetMuzzlePoint()->GetComponentLocation()); 
                            }

                            const float EffectDuration = 1.5f;
                            if (LastShotTime >= ShootCooldown + EffectDuration)
                            {
                                Shoot();
                                Enemy->isShooting = true;
                                LastShotTime = 0.0f;
                                EffectIsPlaying = false;
                            }
                        }
                    }
                    else
                    {
                        Enemy->isShooting = false;
                    }
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

void AShooterBossAIController::OnNiagaraSystemFinished(UNiagaraComponent* NiagaraComponent)
{

}