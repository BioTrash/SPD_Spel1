// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTurret.h"
#include "PlayerCharacter.h"
#include "MathUtil.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Projectile.h"
#include "Kismet/KismetMathLibrary.h"

//Hanna
AEnemyTurret::AEnemyTurret()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComponent;
    
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComponent);
    
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh ->SetupAttachment(BaseMesh);
    
	ProjectileSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Projectile"));
	ProjectileSpawn -> SetupAttachment(TurretMesh);
}
//Körs när spelet startar eller spawnas in
void AEnemyTurret::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}
//Körs varje frame
void AEnemyTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Health <= 0)
	{
		Die();
		return;
	}

	if (Player)
	{
		float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
		if (Distance <= FireRange)
		{
			ShootEnemy(10.0f);
			ShootProjectiles();
			RotateTurret(Player->GetActorLocation());
		}
	}
}
//Bind functionality to input
void AEnemyTurret::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
//Gör att spelaren kan ta skada
float AEnemyTurret::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToMake = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToMake = FMath::Min(Health,DamageToMake);
	Health -= DamageToMake;
	return DamageToMake;
}
//Rotarerar turretens huvud för att alltid kolla på target location vilket är spelaren
void AEnemyTurret::RotateTurret(FVector TargetLocation)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(TurretMesh->GetComponentLocation(), TargetLocation);
	LookAtRotation.Pitch = 0;
	LookAtRotation.Roll = 0;
	LookAtRotation.Yaw += -90.f; 
	TurretMesh->SetWorldRotation(LookAtRotation);
}
//Skjuter projectiler från turreten
void AEnemyTurret::ShootProjectiles()
{
	if (GetWorld()->GetTimeSeconds() >= NextProjectileTime)
	{
		if (ProjectileClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			FVector SpawnLocation = ProjectileSpawn->GetComponentLocation();
			FRotator SpawnRotation = ProjectileSpawn->GetComponentRotation();
			
			if (AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams))
			{
				Projectile->SetOwner(this);
			}
		}
		NextProjectileTime = GetWorld()->GetTimeSeconds() + ProjectileSpawnCooldown;
	}
}
//Skjuter på spelaren med insant-hit
void AEnemyTurret::ShootEnemy(float Damage)
{
	if (GetWorld()->GetTimeSeconds() >= NextShootTime)
	{
		if (Player)
		{
			float DistanceToPlayer = FVector::Distance(GetActorLocation(), Player->GetActorLocation());
			float DistanceMultiplier = FMath::Clamp(1.0f - (DistanceToPlayer * DamageRadius), 0.0f, 1.0f);
			float ActualDamage = Damage * DistanceMultiplier + 8.f;

			Player->TakeDamage(ActualDamage, FDamageEvent(), GetInstigatorController(), this);
			NextShootTime = GetWorld()->GetTimeSeconds() + ShootCooldown;
		}
		ShootAgainCooldown();
		}
	IsShootingAnimation = false;
	}
//Ser till att spelaren dör
void AEnemyTurret::Die()
{
	OnEnemyDeath();
	// Gömmer turret istället för att destroya.
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	//Destroy();
}
//Resettar cooldownen på när spelaren ska skjuta igen
void AEnemyTurret::ShootAgainCooldown()
{
	NextShootTime = GetWorld()->GetTimeSeconds() + ShootCooldown;
}
//Animatoner för Nora
bool AEnemyTurret::GetIsShootingAnimation()
{
	return IsShootingAnimation;
}
