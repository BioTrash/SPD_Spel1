// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyTurret.h"
#include "PlayerCharacter.h"
#include "MathUtil.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Projectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/DamageEvents.h"

//Hanna
AEnemyTurret::AEnemyTurret()
{
	PrimaryActorTick.bCanEverTick = true;
	InitializeTurretComponents();
}
void AEnemyTurret::InitializeTurretComponents()
{
	//Skapar alla components för turreten och sätter de som attachments där de behövs
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComponent;
    
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComponent);
        
	TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh ->SetupAttachment(BaseMesh);
    	
	ProjectileSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Projectile"));
	ProjectileSpawn->SetupAttachment(TurretMesh);
}

//Körs när spelet startar eller spawnas in
void AEnemyTurret::BeginPlay()
{
	Super::BeginPlay();
	//Sätter turretens health till maxhealth
	Health = MaxHealth;
	//Hittar spelarens karaktär och lägger en referens till den
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}
//Körs varje frame
void AEnemyTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Kollar om health är noll eller lägre 
	if (Health <= 0)
	{
		//Anropar diefunktionen och returnerar
		HandleDeath();
		return;
	}

	//Kollar om spelaren existerar
	if (Player)
	{
		//Beräknar avståndet mellan turreten och spelaren
		float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
		//Kollar om spelaren är inom firerange(skjutavstånd)
		if (Distance <= FireRange)
		{
			//Skjuter på spelaren med projektiler och roterar även turretens huvud mot spelaren
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
	//Beräknar ut rotationen för att hitta targetlocation
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(TurretMesh->GetComponentLocation(), TargetLocation);
	//nollställer Pitch och roll, justera yaw så den kan kolla på spelaren hela tiden
	LookAtRotation.Pitch = 0;
	LookAtRotation.Roll = 0;
	LookAtRotation.Yaw += -90.f;
	//Sätter turretens rotation till den beröknande rotationen
	TurretMesh->SetWorldRotation(LookAtRotation);
}
//Skjuter projectiler från turreten
void AEnemyTurret::ShootProjectiles()
{
	
	//Kollar cooldown tiden innan den skjuter
	if (GetWorld()->GetTimeSeconds() >= NextProjectileTime)
	{
			//Ställer in parametrar för att spawna in projektiler
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			//Hämtar spawn positionen och rotationen 
			FVector SpawnLocation = ProjectileSpawn->GetComponentLocation();
			FRotator SpawnRotation = ProjectileSpawn->GetComponentRotation();

			//Spawnar in projektiler i världen
			if (AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams))
			{
				//Sätter projektiles ägare till turreten själv(this)
				Projectile->SetOwner(this);
				Projectile->SetDamage(10);
			}
		//Sätter nästa tid som en projektil ska skjutas
		NextProjectileTime = GetWorld()->GetTimeSeconds() + ProjectileSpawnCooldown;
	}
}

//Skjuter på spelaren med insant-hit
void AEnemyTurret::ShootEnemy(float Damage)
{
	//Kontrollerar om cooldown tiden har gått
	if (GetWorld()->GetTimeSeconds() >= NextShootTime)
	{
			//Beröknar avståndet till spelaren
			float DistanceToPlayer = FVector::Distance(GetActorLocation(), Player->GetActorLocation());
			//Beräknar skadans multiplier beroende på avståndet
			float DistanceMultiplier = FMath::Clamp(1.0f - (DistanceToPlayer * DamageRadius), 0.0f, 1.0f);
			//Beräknar sedan den faktiska skadan som ska göras
			float ActualDamage = Damage * DistanceMultiplier + 8.f;
			//Gör skada på spelaren
			Player->TakeDamage(ActualDamage, FDamageEvent(), GetInstigatorController(), this);
			//Sätter tiden som nästa skott ska göras
			NextShootTime = GetWorld()->GetTimeSeconds() + ShootCooldown;
			//resettar cooldown-tiden för skjutandet
			ShootAgainCooldown();
			//Sätter animationen till false
			IsShootingAnimation = false;
	}
}

//Ser till att spelaren dör
void AEnemyTurret::HandleDeath()
{
	OnEnemyDeath();
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
// Resetta HP (Jeremy)
void AEnemyTurret::ResetHealth()
{
	Health = MaxHealth;
}
