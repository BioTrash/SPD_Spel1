
#include "EnemyShooterAIController.h"
#include "EnemyWeapon.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterEnemy.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "EnemyCommunicationManager.h"

#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "EnemyWeapon.h"

void AEnemyShooterAIController::BeginPlay()
{
    Super::BeginPlay();

    UEnemyCommunicationManager* CommunicationManager = UEnemyCommunicationManager::GetInstance();
    if (CommunicationManager)
    {
        CommunicationManager->OnPlayerLocationUpdated.AddDynamic(this, &AEnemyShooterAIController::OnPlayerLocationUpdated);
        CommunicationManager->OnChaseUpdated.AddDynamic(this, &AEnemyShooterAIController::OnChaseUpdated);
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
        GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
        GetBlackboardComponent()->SetValueAsBool(TEXT("HasRushed"), false);

    }
    SetFocus(PlayerPawn);
}

void AEnemyShooterAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    LastShotTime += DeltaSeconds;
    UE_LOG(LogTemp, Warning, TEXT("LASTSHOTTIME: %f"), LastShotTime);
    if (PlayerPawn != nullptr)
    {
        SetFocus(PlayerPawn);
    }
    bool ChaseStatus = GetBlackboardComponent()->GetValueAsBool(TEXT("Chase"));
    Enemy = Cast<AShooterEnemy>(GetPawn());
    if (Enemy)
    {
        EnemyWeapon = Enemy->TriggerWeapon; //OBS! Ska bort. Enbart för att AI skapas innan Shooterenemy och därav är triggerweapon null i första beginplay().
        DirectionToPlayer = PlayerPawn->GetActorLocation() - Enemy->GetActorLocation();
        WeaponRotation = DirectionToPlayer.Rotation();

        GetBlackboardComponent()->SetValueAsFloat(TEXT("Health"), Enemy->Health);

        if (EnemyWeapon)
        {
            //Räknar ut avståndet mellan spelaren och fienden
            FVector PlayerLocation = PlayerPawn->GetActorLocation();
            EnemyLocation = Enemy->GetActorLocation();
            float DistanceToPlayer = FVector::Distance(EnemyLocation, PlayerLocation);
            
            if (DistanceToPlayer < 500.0f)
            {
                //Logik bakom ny position att röra sig mot om fienden befinner sig för nära
                GetBlackboardComponent()->SetValueAsBool(TEXT("InPlayerRange"), true);
                FVector DirectionAwayFromPlayer = EnemyLocation - PlayerLocation;
                DirectionAwayFromPlayer.Normalize();

                FVector NewDestination = EnemyLocation + (DirectionAwayFromPlayer * 500.0f);
                GetBlackboardComponent()->SetValueAsVector(TEXT("BackOffLocation"), NewDestination);
            }
            else
            {
                GetBlackboardComponent()->SetValueAsBool(TEXT("InPlayerRange"), false);
            }

            // "Rage"-läge vid specifikt HP
            if (Enemy->Health >= 30 && Enemy->Health < 50)
            {
                GetBlackboardComponent()->SetValueAsBool(TEXT("HasRushed"), true);
            }
            
                // Vektorer för Trace (Louis)
                FVector StartTrace = EnemyWeapon->GetActorLocation();
                FVector EndTrace = PlayerPawn->GetActorLocation();

                //Parametrar för linetrace (Louis)
                FHitResult HitResult;
                FCollisionQueryParams CollisionParams;
                CollisionParams.AddIgnoredActor(Enemy);
            
                // Line trace (Louis)
                if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_GameTraceChannel1, CollisionParams))
                {
                    
                    // Check ray träffar spelare (Louis)
                    if (HitResult.GetActor() == PlayerPawn && !HitResult.GetActor()->ActorHasTag("Enemy"))
                    {
                    
                        // Delegatuppdatering för andra shooters
                        DetectPlayer((HitResult.GetActor()->GetActorLocation()));
                        BeginChase(true);

                        //Skapa ett random Reposition-location som kan användas 

                        UpdateRePositionLocation();
                        ShootTime = true;
                        //Check för skottcooldowns samt effekt
                        if (LastShotTime >= ShootCooldown && ShootTime)
                        {
                            Enemy->IsCharging = true;
                            const float EffectDuration = 1.5f;
                            
                            if (LastShotTime >= ShootCooldown + EffectDuration)
                            {
                                //Skjut-logik
                                Enemy->IsCharging = false;
                                Shoot();
                                UpdateRePositionLocation();

                                //Används bl.a för animation
                                Enemy->isShooting = true;
                                LastShotTime = 0.0f;
                                EffectIsPlaying = false;
                                BeginChase(false);
                                GetBlackboardComponent()->SetValueAsBool(TEXT("IsShooting"), false);
                                ShootTime = false;
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
    // Få ett unikt ID för Enemy-objektet via GetUniqueID
    FString EnemyID = FString::Printf(TEXT("%d"), GetUniqueID());

    // Få ett random Hash baserat på ID-t. Används för att fienden ska få en slumpmässig men återskaplig randomlocation 
    uint32 Seed = FCrc::StrCrc32(*EnemyID);
    FMath::RandInit(Seed);

    // Generera ny slumpmässig position att uppdatera PlayerLocation med för andra
    float Radius = 1500.0f;
    FVector2D RandomOffset = FMath::RandPointInCircle(Radius);
    FVector RandomPlayerLocation = PlayerLocation + FVector(RandomOffset.X, RandomOffset.Y, 0.0f);

    // Uppdatera PlayerLocation genom delegatet
    UEnemyCommunicationManager::GetInstance()->SetPlayerLocation(RandomPlayerLocation);
    UEnemyCommunicationManager::GetInstance()->SetIsChasing(true);
}


void AEnemyShooterAIController::OnPlayerLocationUpdated(const FVector& NewPlayerLocation)
{
    GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), NewPlayerLocation);
    UE_LOG(LogTemp, Error, TEXT("MIN PLAYER POS ÄR: %s"), *GetBlackboardComponent()->GetValueAsVector(TEXT("PlayerLocation")).ToString());

}

void AEnemyShooterAIController::OnChaseUpdated(const bool NewChaseStatus)
{
    GetBlackboardComponent()->SetValueAsBool(TEXT("Chase"), NewChaseStatus);
    UE_LOG(LogTemp, Error, TEXT("CHASE!!!:"));
}

void AEnemyShooterAIController::BeginChase(bool ChaseStatus)
{
    UEnemyCommunicationManager::GetInstance()->SetIsChasing(ChaseStatus);
}

void AEnemyShooterAIController::Shoot()
{
    EnemyWeapon->ShootEnemyProjectile();
}

// Säkerställ att fiende har referens till en själv. Fiende måste ha ett vapen och instans för att initieras
void AEnemyShooterAIController::InitiateEnemy()
{
    Enemy = Cast<AShooterEnemy>(GetPawn());
    if (Enemy)
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

void AEnemyShooterAIController::UpdateRePositionLocation()
{
    FVector LastKnownPlayerLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("PlayerLocation"));
    float Radius = 900.0f;
    FVector2D RandomOffset = FMath::RandPointInCircle(Radius);
    FVector RePositionLocation = LastKnownPlayerLocation + FVector(RandomOffset.X, RandomOffset.Y, 0.0f);
    GetBlackboardComponent()->SetValueAsVector(TEXT("RePositionLocation"), RePositionLocation);

}

// Säkerställ att fiende har referens till spelaren
void AEnemyShooterAIController::InitiatePlayer()
{
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}