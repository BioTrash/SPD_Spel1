#include "ShooterEnemy.h"
#include "ProjectileWeapon.h"
#include "Components/PointLightComponent.h"
#include "EnemyShooterAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

AShooterEnemy::AShooterEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShooterEnemy::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	isAlive = true;
	if (BP_EnemyWeaponClass)
	{
		// Spawna vapnet
		AProjectileWeapon* WeaponInstance = GetWorld()->SpawnActor<AProjectileWeapon>(BP_EnemyWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);

		// Checka spawnen
		if (WeaponInstance)
		{
			// Attacha vapnet i rätt socket
			WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("EnemyWeaponSocket"));
            
			// Set ägare fär vapnet
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

void AShooterEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// Används för att rensa fiende
	DeathTime += DeltaSeconds;

	// Om död
	if(Health <= 0 && isAlive)
	{
		TriggerWeapon->Destroy();
		KillEnemy();
	}
	// Om död och dags att rensa
	if (DeathTime >= DespawnCooldown && !isAlive)
	{
		Destroy(); // Att byta ut mot Object Pooling-logik
	}
}

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
	return DamageToMake;
}

void AShooterEnemy::KillEnemy()
{
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
}

UStaticMeshComponent* AShooterEnemy::GetStaticMeshComponent() const
{
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
		
		float ImpulseStrength = 6000;
		SkeletalMesh->AddImpulse(HitDirection * ImpulseStrength , HitBoneName, true);
	}
}

bool AShooterEnemy::getIsShooting()
{
	return isShooting;
}

// Används vid varje träff från spelare för att logga vad som träffats. Används i Ragdolling
void AShooterEnemy::SetHitInformation(FName BoneName, FVector Direction)
{
	HitBoneName = BoneName;
	HitDirection = Direction;

}