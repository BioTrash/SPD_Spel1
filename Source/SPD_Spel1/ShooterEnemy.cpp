#include "ShooterEnemy.h"
#include "ProjectileWeapon.h"
#include "Components/PointLightComponent.h"
#include "EnemyShooterAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

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
	isAlive = true;
	if (BP_EnemyWeaponClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BP INITIATED"));
		// Spawn the BP_EnemyProjectileWeapon
		AProjectileWeapon* WeaponInstance = GetWorld()->SpawnActor<AProjectileWeapon>(BP_EnemyWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);

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
	}
}

// Called every frame
void AShooterEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	DeathTime += DeltaSeconds;
	if(Health <= 0 && isAlive)
	{
		TriggerWeapon->Destroy();
		KillEnemy();
	}
	if (DeathTime >= DespawnCooldown && !isAlive)
	{
		Destroy();
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

	if(Health > 0)
	{
		IsTakingDamage = true;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Health left: %f"), Health);
	IsTakingDamage = false;
	return DamageToMake;
}

void AShooterEnemy::KillEnemy()
{
	//För att Jeremy ska kunna hantera Death i sin EnemySpawn(Hanna)
	SetRagdollPhysics();
	OnEnemyDeath();
	isAlive = false;
	DeathTime = 0;
	TriggerWeapon->SetActorTickEnabled(false);
	TriggerWeapon->SetOwner(nullptr);
	AAIController* EnemyAIController = Cast<AAIController>(GetController());
	if (EnemyAIController)
	{
		EnemyAIController->StopMovement();
		EnemyAIController->UnPossess();
		EnemyAIController->Destroy();
		//EnemyAIController->SetControlledPawn(nullptr);
	}
	UPointLightComponent* PointLightComponent = FindComponentByClass<UPointLightComponent>();
	if (PointLightComponent)
	{
		PointLightComponent->DestroyComponent();
	}
	UPrimitiveComponent* EnemyRootComponent = Cast<UPrimitiveComponent>(GetRootComponent());
	UCapsuleComponent* EnemyCapsuleComponent = GetCapsuleComponent();
	if (EnemyCapsuleComponent)
	{
		EnemyCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		USphereComponent* HeadshotComponent = nullptr;
		TArray<USceneComponent*> ChildrenList;
		EnemyCapsuleComponent->GetChildrenComponents(true, ChildrenList);
		for (USceneComponent* Child : ChildrenList)
		{
			if (USphereComponent* SphereChild = Cast<USphereComponent>(Child))
			{
				if (SphereChild->GetName() == "Headshot")
				{
					HeadshotComponent = SphereChild;
					break;
				}
			}
		}

		if (HeadshotComponent)
		{
			HeadshotComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
	//Destroy();
}

UStaticMeshComponent* AShooterEnemy::GetStaticMeshComponent() const
{
	// Assuming the static mesh component is named "EnemyStaticMesh"
	return FindComponentByClass<UStaticMeshComponent>();
}

void AShooterEnemy::SetRagdollPhysics()
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh)
	{
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SkeletalMesh->SetCollisionProfileName(TEXT("Ragdoll"));

		SkeletalMesh->SetAllBodiesSimulatePhysics(true);
		SkeletalMesh->WakeAllRigidBodies();
		
		float ImpulseStrength = 8000;
		float UpwardImpulseStrength = 1000;
		if(HitBySlime)
		{
			FVector TotalImpulse = HitDirection * ImpulseStrength + FVector(0, 0, UpwardImpulseStrength);
			SkeletalMesh->AddImpulse(TotalImpulse , NAME_None, true);
			UE_LOG(LogTemp, Warning, TEXT("HIT BY SLIME"));
		}
		else
		{
			SkeletalMesh->AddImpulse(HitDirection * ImpulseStrength , HitBoneName, true);
			UE_LOG(LogTemp, Warning, TEXT("HIT BY SHOT"));

		}
	}
}

bool AShooterEnemy::getIsShooting()
{
	return isShooting;
}

bool AShooterEnemy::GetIsCharging()
{
	return IsCharging;
}

bool AShooterEnemy::GetIsTakingDamage()
{
	return IsTakingDamage;
}

void AShooterEnemy::SetHitInformation(FName BoneName, FVector Direction, bool SlimeShot)
{
	HitBoneName = BoneName;
	HitDirection = Direction;
	HitBySlime = SlimeShot;
}