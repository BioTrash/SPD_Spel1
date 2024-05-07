// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTurret.h"
#include "PlayerCharacter.h"
#include "MathUtil.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemyTurret::AEnemyTurret()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComponent;
    
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComponent);
    
	TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh ->SetupAttachment(BaseMesh);
    
	ProjectileSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Projectile"));
	ProjectileSpawn -> SetupAttachment(TurretMesh);
	
}
void AEnemyTurret::BeginPlay()
{
	Super::BeginPlay();
	ShootingAnimation();
	FVector TurretMeshLocation = TurretMesh->GetRelativeLocation();
	TurretMesh->SetRelativeLocation(TurretMeshLocation);
	Health = MaxHealth;
	
	APlayerCharacter* FoundPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	if (FoundPlayer)
	{
		Player = FoundPlayer;
	}
}
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
			UE_LOG(LogTemp, Warning, TEXT("Animation körs"));
			//ShootingAnimation();
			PerformLineTrace();
			RotateTurret(Player->GetActorLocation());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Animation körs"));
			//IdleAnimation(); 
		}
	}
}
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
}
void AEnemyTurret::PerformLineTrace()
{
	FVector StartLocation = TurretMesh->GetComponentLocation(); 
	FVector EndLocation = Player->GetActorLocation();

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); 

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, TraceChannel, Params);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 0.1f, 0, 2);

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Nu ska jag skjuta!"));
		ShootEnemy(10.0f);
	}
}
void AEnemyTurret::ShootEnemy(float Damage)
{
	UE_LOG(LogTemp, Warning, TEXT("KÖRS DU?!"));
	if (GetWorld()->GetTimeSeconds() >= NextShootTime)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (PlayerCharacter)
		{
			float DistanceToPlayer = FVector::Distance(GetActorLocation(), PlayerCharacter->GetActorLocation());
			float DistanceMultiplier = FMath::Clamp(1.0f - (DistanceToPlayer * DamageRadius), 0.0f, 1.0f);
			float ActualDamage = Damage * DistanceMultiplier + 8.f;

			PlayerCharacter->TakeDamage(ActualDamage, FDamageEvent(), GetInstigatorController(), this);
			NextShootTime = GetWorld()->GetTimeSeconds() + ShootCooldown;
		}
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