// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Projectile.h"
#include "EnemyWeapon.generated.h"


/**
 * 
 */
UCLASS()
class SPD_SPEL1_API AEnemyWeapon : public AWeapon
{
	GENERATED_BODY()

public:

	void FireAtPlayer(float DamageDone);
	//void PullTrigger();

protected:

	void BeginPlay() override;

private:
	//virtual void ShootProjectile();
	
    UPROPERTY()
    AWeapon* WeaponComponent;

	UPROPERTY(EditAnywhere, Category= "Projectile")
	TSubclassOf<class AProjectile> Bullet;
};
