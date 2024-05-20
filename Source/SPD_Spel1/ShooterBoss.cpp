#include "ShooterBoss.h"
#include "ProjectileWeapon.h"
#include "Components/PointLightComponent.h"
#include "EnemyShooterAIController.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"


AShooterBoss::AShooterBoss()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShooterBoss::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	isAlive = true;
	if (BP_EnemyWeaponClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BP INITIATED"));
		AProjectileWeapon* WeaponInstance = GetWorld()->SpawnActor<AProjectileWeapon>(BP_EnemyWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);

		if (WeaponInstance)
		{
			UE_LOG(LogTemp, Error, TEXT("Instance good"));

			WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
            
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

void AShooterBoss::Tick(float DeltaSeconds)
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

void AShooterBoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AShooterBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToMake = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToMake = FMath::Min(Health,DamageToMake);
	Health -= DamageToMake;
	return DamageToMake;
}

void AShooterBoss::KillEnemy()
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

UStaticMeshComponent* AShooterBoss::GetStaticMeshComponent() const
{
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
		
		float ImpulseStrength = 6000;
		SkeletalMesh->AddImpulse(HitDirection * ImpulseStrength , HitBoneName, true);
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