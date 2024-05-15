// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SPD_SPEL1_API AProjectileWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	AProjectileWeapon();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Shoot() override;
	void ShootEnemyProjectile();

	// The inbuilt but NOT obligatory way of shooting, more info in the .cpp file. Made initially for player shooting but can be adapted for AI (Rufus)
	virtual void InitiateTimer(bool bButtonPressed, bool bAlternative) override;

	// Preferably Projectile needs to be made into an interface and TSubclassOf should be called a base class. But if enemies are only using one type of projectiles this is fine (Rufus)
	UPROPERTY(EditAnywhere, Category="Weaponry")
	TSubclassOf<class AProjectile> Projectile;

	UPROPERTY(VisibleAnywhere)
	AProjectile* SpawnedProjectile;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;
};
