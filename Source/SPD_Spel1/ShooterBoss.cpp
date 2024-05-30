#include "ShooterBoss.h"
#include "ProjectileWeapon.h"
#include "Components/PointLightComponent.h"
#include "EnemyShooterAIController.h"
#include "ShooterBossAIController.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AShooterBoss::AShooterBoss()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AShooterBoss::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	isAlive = true;

	if (BP_EnemyWeaponClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BP INITIATED"));
		// Spawn the BP_EnemyProjectileWeapon
		WeaponInstance = GetWorld()->SpawnActor<AProjectileWeapon>(BP_EnemyWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);

		// Check if spawn was successful
		if (WeaponInstance)
		{
			UE_LOG(LogTemp, Error, TEXT("Instance good"));

			// Attach the weapon to the mesh socket or root
			WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("EnemyWeaponSocket"));
            
			// Set the owner of the weapon
			WeaponInstance->SetOwner(this);
			TriggerWeapon = WeaponInstance;
			TriggerWeapon->SetOwner(this);
			if (TriggerWeapon)
			{
				UE_LOG(LogTemp, Error, TEXT("Instance TRIGGER good"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Instance NO good"));

		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Class NO good"));

	}
}

// Called every frame
void AShooterBoss::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(Health <= 0 && isAlive)
	{
		//TriggerWeapon->Destroy();
		
		KillEnemy();
	}
}

// Called to bind functionality to input
void AShooterBoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AShooterBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToMake = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//to make sure that the DamageToMake is not greater than the health we have left, therefore we make the DamageToMake to be the amount we have left (Rebecka) 
	DamageToMake = FMath::Min(Health,DamageToMake);
	Health -= DamageToMake;
	//UE_LOG(LogTemp, Warning, TEXT("Health left: %f"), Health);
	return DamageToMake;
}

void AShooterBoss::KillEnemy()
{
	//För att Jeremy ska kunna hantera Death i sin EnemySpawn(Hanna)
	SetRagdollPhysics();
	OnEnemyDeath();
	isAlive = false;
	TriggerWeapon->SetActorHiddenInGame(true);
	TriggerWeapon->SetActorTickEnabled(false);
}

UStaticMeshComponent* AShooterBoss::GetStaticMeshComponent() const
{
	// Assuming the static mesh component is named "EnemyStaticMesh"
	return FindComponentByClass<UStaticMeshComponent>();
}

void AShooterBoss::SetRagdollPhysics()
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh)
	{
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SkeletalMesh->SetCollisionProfileName(TEXT("Ragdoll"));

		SkeletalMesh->SetAllBodiesSimulatePhysics(true);
		SkeletalMesh->WakeAllRigidBodies();
		
		float ImpulseStrength = 12000;
		SkeletalMesh->AddImpulse(HitDirection * ImpulseStrength , HitBoneName, true);
	}
}

void AShooterBoss::ResetRagdollPhysics()
{
	isAlive = true;
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh)
	{
		// Disable physics simulation and re-enable animation
		SkeletalMesh->SetAllBodiesSimulatePhysics(false);
		SkeletalMesh->bBlendPhysics = false;  // Disable blending between physics and animation

		// Reset collision settings to the default
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkeletalMesh->SetCollisionProfileName(TEXT("CharacterMesh"));

		// Reset the skeletal mesh transform to its initial state (if needed)
		SkeletalMesh->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// Optionally, you might need to reset the animation
		UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Stop(0.0f);
		}

		// Force update the bone transforms and animation state
		SkeletalMesh->RefreshBoneTransforms();
		SkeletalMesh->UpdateComponentToWorld();
		SkeletalMesh->TickAnimation(0.0f, false);

		// Wake up all rigid bodies to ensure they're in the correct state
		SkeletalMesh->WakeAllRigidBodies();

		SkeletalMesh->SetRelativeLocation(FVector(0, 0, -90));
		SkeletalMesh->SetRelativeRotation(FRotator(0, -90, 0));
	}
}

bool AShooterBoss::getIsShooting()
{
	return isShooting;
}

void AShooterBoss::SetHitInformation(FName BoneName, FVector Direction)
{
	HitBoneName = BoneName;
	HitDirection = Direction;

}

void AShooterBoss::ResetHealth()
{
	Health = MaxHealth;
}

float AShooterBoss::GetHealth() const
{
	return Health; 
}

float AShooterBoss::GetHealthPercentage() const
{
	return Health / MaxHealth;
}

AProjectileWeapon* AShooterBoss::GetTriggerWeapon() const
{
	if(TriggerWeapon)
	{
		return TriggerWeapon;
	}
	return nullptr;
}



