// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTurret.h"
#include "PlayerCharacter.h"
#include "MathUtil.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/EngineTypes.h"

// Sets default values
AEnemyTurret::AEnemyTurret()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

// Called when the game starts or when spawned
void AEnemyTurret::BeginPlay()
{
	Super::BeginPlay();
	FVector TurretMeshLocation = TurretMesh->GetRelativeLocation();
	//TurretMeshLocation.Z += 20.f;
	TurretMesh->SetRelativeLocation(TurretMeshLocation);
	Health = MaxHealth;
	
	APlayerCharacter* FoundPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	if (FoundPlayer)
	{
		Player = FoundPlayer;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player character not found!"));
	}
}

// Called every frame
void AEnemyTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Health <= 0)
	{
		Die();
	}
	if (Player)
	{
		float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("Distance to player: %f"), Distance);
		//DrawDebugLine(GetWorld(), TurretMesh->GetComponentLocation(), Player->GetActorLocation(), FColor::Green, false, 0.1f, 0, 1);

		if (Distance <= FireRange)
		{
			PerformLineTrace();
			RotateTurret(Player->GetActorLocation());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Player is out of firing range")); // Debugging out of range
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player reference is not valid")); // Debugging player reference
	}
	}


// Called to bind functionality to input
void AEnemyTurret::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEnemyTurret::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToMake = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//to make sure that the DamageToMake is not greater than the health we have left, therefore we make the DamageToMake to be the amount we have left (Rebecka) 
	DamageToMake = FMath::Min(Health,DamageToMake);
	Health -= DamageToMake;
	UE_LOG(LogTemp, Warning, TEXT("Health left: %f"), Health);
	return DamageToMake;
}


void AEnemyTurret::RotateTurret(FVector TargetLocation)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(TurretMesh->GetComponentLocation(), TargetLocation);
	LookAtRotation.Pitch = 0;
	LookAtRotation.Roll = 0;
	LookAtRotation.Yaw += -90.f; 
	TurretMesh->SetWorldRotation(LookAtRotation);
	
	UE_LOG(LogTemp, Warning, TEXT("Turret rotation updated"));
	
}

void AEnemyTurret::PerformLineTrace()
{
	FVector StartLocation = TurretMesh->GetComponentLocation(); // Start from the turret's position
	FVector EndLocation = Player->GetActorLocation(); // Trace towards the player's position

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); 

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, TraceChannel, Params);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 0.1f, 0, 2);

	if (bHit)
	{
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(HitResult.GetActor()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player detected by line trace"));
			ShootEnemy(10.0f);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace did not hit the player"));
	}
}

	void AEnemyTurret::ShootEnemy(float Damage)
	{
	if (GetWorld()->GetTimeSeconds() >= NextShootTime)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (PlayerCharacter)
		{
			float DistanceToPlayer = FVector::Distance(GetActorLocation(), PlayerCharacter->GetActorLocation());
			float DistanceMultiplier = FMath::Clamp(1.0f - (DistanceToPlayer * DamageRadius), 0.0f, 1.0f);
			float ActualDamage = Damage * DistanceMultiplier + 8.f;
			float DamageApplied = PlayerCharacter->TakeDamage(ActualDamage, FDamageEvent(), GetController(), this);
			
			NextShootTime = GetWorld()->GetTimeSeconds() + ShootCooldown;
		}

		UE_LOG(LogTemp, Warning, TEXT("Enemy did damage!"));
		
		ShootAgainCooldown();
	}
	}

void AEnemyTurret::Die()
{
	OnEnemyDeath();
	Destroy();
}
void AEnemyTurret::ShootAgainCooldown()
{
	NextShootTime = GetWorld()->GetTimeSeconds() + ShootCooldown;
}


