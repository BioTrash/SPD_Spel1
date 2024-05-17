// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Projectile.h"
#include "NonProjectileWeapon.h"

AProjectileWeapon::AProjectileWeapon()
{
	/*
	 * BeginPlay() and Shoot() are using object pooling of a single Projectile for more efficient memory management (Rufus)
	 */
}

void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();

	SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(Projectile, GetMuzzlePoint()->GetComponentLocation(), GetMuzzlePoint()->GetComponentRotation());
	
	SpawnedProjectile->SetActorHiddenInGame(true);
	SpawnedProjectile->SetActorEnableCollision(false);
	SpawnedProjectile->SetActorTickEnabled(false);

	UProjectileMovementComponent* TempMove = SpawnedProjectile->GetProjectileMovementComponent();
	TempMove->SetActive(false);
	SpawnedProjectile->SetProjectileMovementComponent(TempMove);
}


void AProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileWeapon::Shoot()
{
	SpawnedProjectile->SetActorLocation(GetMuzzlePoint()->GetComponentLocation());
	SpawnedProjectile->SetActorRotation(GetMuzzlePoint()->GetComponentRotation());

	SpawnedProjectile->SetActorHiddenInGame(false);
	SpawnedProjectile->SetActorEnableCollision(true);
	SpawnedProjectile->SetActorTickEnabled(true);

	UProjectileMovementComponent* TempMove = SpawnedProjectile->GetProjectileMovementComponent();
	TempMove->SetActive(true);
	
	SpawnedProjectile->SetProjectileMovementComponent(TempMove);
}

void AProjectileWeapon::InitiateTimer(bool bButtonPressed, bool bAlternative)
{
	/* TL;DR: DISREGARD THIS FUNCTION ENTIRELY IF YOU PREFER TO TRIGGER SHOOT() FUNCTION VIA YOUR OWN FUNCTIONS (Rufus)
	 *
	 * InitiateTimer activates a timer in the WeaponBase, this gives you access to ReloadDelay (Not important for enemies)
	 * and FireDelay (Important for enemies). The latter of which handles how often the weapon can shoot at most, which is mot likely how often enemies will shoot.
	 * Modified in the blueprint (BP_EnemyProjectileWeapon -> Fire Delay)
	 *
	 * bButtonPressed is what activates the timer in PullTrigger when TRUE. bAlternative is always FALSE unless you want to create an alternative Shoot() function and call it instead.
	 * &AProjectileWeapon::Shoot is the function that you want to be triggered every once per FireDelay. THIS is there because timer need to know where the function comes from.
	 *
	 * EXAMPLE USE:
	 * In a certain EnemyClass.h create AWeaponBase* TriggerWeapon; that you make (EditableAnywhere) and set the weapon in Blueprints.
	 * In a certain EnemyClass.cpp you most likely have a part that check if enemy sees the player. In that part you call TriggerWeapon->InitiateTimer(true, false); every time player is seen.
	 * 
	 */
	Super::PullTrigger(bButtonPressed, bAlternative, &AProjectileWeapon::Shoot, this);
}

