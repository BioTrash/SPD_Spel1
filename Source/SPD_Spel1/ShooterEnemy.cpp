// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterEnemy.h"
#include "Weapon.h"


// Sets default values
AShooterEnemy::AShooterEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterEnemy::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;

	//WeaponComponent = FindComponentByClass<UBP_Weapon_Component>();
	//WeaponComponent = FindComponentByClass<UBP_Weapon_Component>();
	if (WeaponClass)
	{
		// Spawn the weapon
		AWeapon* WeaponInstance = GetWorld()->SpawnActor<AWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);

		// Check if spawn was successful
		if (WeaponInstance)
		{
			// Attach the weapon to the mesh socket
			WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));

			// Set the owner of the weapon
			WeaponInstance->SetOwner(this);

			// Store a reference to the weapon
			TriggerWeapon = WeaponInstance;

			TriggerWeapon->SetOwner(this);
		}
	}
}

// Called every frame
void AShooterEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Health <= 0)
	{
		KillEnemy();
	}

}

// Called to bind functionality to input
void AShooterEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AShooterEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToMake = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//to make sure that the DamageToMake is not greater than the health we have left, therefore we make the DamageToMake to be the amount we have left (Rebecka) 
	DamageToMake = FMath::Min(Health,DamageToMake);
	Health -= DamageToMake;
	UE_LOG(LogTemp, Warning, TEXT("Health left: %f"), Health);
	return DamageToMake;
}

void AShooterEnemy::KillEnemy()
{
	UE_LOG(LogTemp, Warning, TEXT("ENEMY SHOULD DIE"));
	
	//För att Jeremy ska kunna hantera Death i sin EnemySpawn(Hanna)
	OnEnemyDeath();
	TriggerWeapon->Destroy();
	Destroy();
}

