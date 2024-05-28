#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "GenericObjectPool.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectileWeapon::AProjectileWeapon()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AProjectileWeapon::BeginPlay()
{
    Super::BeginPlay();
    // Ensure ObjectPool is set here or in the editor
}

void AProjectileWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AProjectileWeapon::Shoot()
{
    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Error, TEXT("Shoot: ProjectileClass is not set"));
        return;
    }

    FVector SpawnLocation = GetMuzzlePoint()->GetComponentLocation();
    FRotator SpawnRotation = GetMuzzlePoint()->GetComponentRotation();

    AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
    if (SpawnedProjectile)
    {
        SpawnedProjectile->SetActorHiddenInGame(false);
        SpawnedProjectile->SetActorEnableCollision(true);
        SpawnedProjectile->SetActorTickEnabled(true);

        UProjectileMovementComponent* TempMove = SpawnedProjectile->GetProjectileMovementComponent();
        TempMove->SetActive(true);
        SpawnedProjectile->SetProjectileMovementComponent(TempMove);
    }
}

void AProjectileWeapon::ShootEnemyProjectile()
{
    if (ObjectPool)
    {
        AProjectile* PooledProjectile = ObjectPool->GetPooledObject<AProjectile>(ProjectileClass);
        if (PooledProjectile)
        {
            PooledProjectile->SetActorLocation(GetMuzzlePoint()->GetComponentLocation());
            PooledProjectile->SetActorRotation(GetMuzzlePoint()->GetComponentRotation());

            PooledProjectile->SetActorHiddenInGame(false);
            PooledProjectile->SetActorEnableCollision(true);
            PooledProjectile->SetActorTickEnabled(true);

            UProjectileMovementComponent* TempMove = PooledProjectile->GetProjectileMovementComponent();
            TempMove->SetActive(true);

            PooledProjectile->SetProjectileMovementComponent(TempMove);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ShootEnemyProjectile: No projectile available in pool"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ShootEnemyProjectile: ObjectPool is not set"));
    }
}

void AProjectileWeapon::InitiateTimer(bool bButtonPressed, bool bAlternative)
{
    // Implement your timer logic here, if needed.
    // This example uses a simple UE_LOG to indicate the function is being called.
    UE_LOG(LogTemp, Log, TEXT("InitiateTimer called with bButtonPressed: %s, bAlternative: %s"), bButtonPressed ? TEXT("True") : TEXT("False"), bAlternative ? TEXT("True") : TEXT("False"));
}
