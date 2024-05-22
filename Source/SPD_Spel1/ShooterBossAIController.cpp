
#include "ShooterBossAIController.h"
#include "EnemyWeapon.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterEnemy.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyCommunicationManager.h"
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
    //ShootEffect->OnSystemFinished.AddDynamic(this, &ASlimeBossAIController::OnNiagaraSystemFinished);
}

void AShooterBossAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (PlayerPawn != nullptr)
    {
        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
        GetBlackboardComponent()->SetValueAsBool(TEXT("InPlayerRange"), false);
        GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
        
        LastShotTime += DeltaSeconds;
        LastSpamShotTime += DeltaSeconds;

        Enemy = Cast<AShooterBoss>(GetPawn());
        if (Enemy)
        {
            EnemyWeapon = Enemy->TriggerWeapon; // Ensure TriggerWeapon is set
            DirectionToPlayer = PlayerPawn->GetActorLocation() - Enemy->GetActorLocation();
            WeaponRotation = DirectionToPlayer.Rotation();
            GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), Enemy->getIsShooting());


            if (EnemyWeapon)
            {
                FVector PlayerLocation = PlayerPawn->GetActorLocation();
                EnemyLocation = EnemyWeapon->GetMuzzlePoint()->GetComponentLocation();
                FVector EnemyBoundsOrigin;
                FVector EnemyBoundsExtent;
                Enemy->GetActorBounds(true, EnemyBoundsOrigin, EnemyBoundsExtent);

                FVector PlayerBoundsOrigin;
                FVector PlayerBoundsExtent;
                PlayerPawn->GetActorBounds(true, PlayerBoundsOrigin, PlayerBoundsExtent);

                float DistanceToPlayer = FVector::Dist(EnemyBoundsOrigin, PlayerBoundsOrigin) - (EnemyBoundsExtent.Size() + PlayerBoundsExtent.Size());
                UE_LOG(LogTemp, Warning, TEXT("RANGE TO PLAYER: %f"), DistanceToPlayer);
                if (DistanceToPlayer < 200.0f)
                {
                    // Logic for new position to move towards if the enemy is too close
                    GetBlackboardComponent()->SetValueAsBool(TEXT("InPlayerRange"), true);
                    FVector DirectionAwayFromPlayer = EnemyLocation - PlayerLocation;
                    DirectionAwayFromPlayer.Normalize();

                    FVector NewDestination = EnemyLocation + (DirectionAwayFromPlayer * 500.0f);
                    GetBlackboardComponent()->SetValueAsVector(TEXT("BackOffLocation"), NewDestination);
                    Enemy->GetCharacterMovement()->MaxWalkSpeed = NewWalkSpeed;

                    if (DistanceToPlayer < 100.0f)
                    {
                        SpamAttack();
                    }
                }
                Enemy->GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed;
                
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
    //JumpBack();
        if (LastSpamShotTime >= SpamShotCooldown && SpamAmount < 5)
        {
            Shoot();
            UE_LOG(LogTemp, Warning, TEXT("SpamFiring!"))
            LastSpamShotTime = 0;
            SpamAmount++;
        }
        if (SpamAmount <= 5)
        {
            SpamAmount = 0;
            InPositionSpam = false;
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
        OriginalWalkSpeed = Enemy->GetCharacterMovement()->MaxWalkSpeed;
        NewWalkSpeed = OriginalWalkSpeed + 300;

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

void AShooterBossAIController::JumpBack()
{
    FVector DirectionAwayFromPlayer = EnemyLocation - PlayerPawn->GetActorLocation();
    DirectionAwayFromPlayer.Normalize();
    FVector NewDestination = EnemyLocation + (DirectionAwayFromPlayer * 700.0f);

    Enemy->SetActorLocation(NewDestination);
    InPositionSpam = true;
    
}

void AShooterBossAIController::InitiatePlayer()
{
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}
