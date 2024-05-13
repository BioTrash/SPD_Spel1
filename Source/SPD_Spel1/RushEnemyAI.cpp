// Fill out your copyright notice in the Description page of Project Settings.

struct FDamageEvent;

#include "RushEnemyAI.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

ARushEnemyAI::ARushEnemyAI()
{
	PrimaryActorTick.bCanEverTick = true;
	bHasExploded = false;
}

void ARushEnemyAI::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

void ARushEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(Health > 0)
	{
		PerformLineTrace();
	}
	if(Health <= 0 && !bHasExploded)
	{
		Explode(30.f, true);
		bHasExploded = true;
	}
}
void ARushEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARushEnemyAI::Explode(float Damage, bool bCollisionTriggered)
{
	UE_LOG(LogTemp, Warning, TEXT("Exploderar i facet >:D"));
	if (bCollisionTriggered && !bHasExploded)
	{
		TArray<AActor*> OverlappingActors;
		FVector ExplosionLocation = GetActorLocation();

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), OverlappingActors);

		for (AActor* Actor : OverlappingActors)
		{
			APlayerCharacter* Player = Cast<APlayerCharacter>(Actor);
			if (Player)
			{
				float DistanceToPlayer = FVector::Distance(ExplosionLocation, Player->GetActorLocation());
				float DamageMultiplier = FMath::Clamp(1.0f - (DistanceToPlayer / DamageRadius), 0.0f, 1.0f);
				float ActualDamage = Damage * DamageMultiplier;

				Player->TakeDamage(ActualDamage, FDamageEvent(), GetController(), this);
			}
		}

	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, ExplosionLocation, FRotator::ZeroRotator, FVector::OneVector, true, true);
	}
		bHasExploded = true;
		UE_LOG(LogTemp, Warning, TEXT("Destroyas"));
		OnEnemyDeath();
		Destroy();
	}
}

void ARushEnemyAI::PerformLineTrace()
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * MaxTraceDistance; 

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); 
    
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Pawn, Params);

	if (bHit)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(HitResult.GetActor());
		if (Player)
		{
			Explode(30, true);
		}
	}
}

void ARushEnemyAI::KillEnemy()
{
	UE_LOG(LogTemp, Warning, TEXT("ENEMY SHOULD DIE"));

	OnEnemyDeathDelegate.Broadcast();
	Health = 0;
	OnEnemyDeath();
	Destroy();
}

float ARushEnemyAI::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToMake = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//to make sure that the DamageToMake is not greater than the health we have left, therefore we make the DamageToMake to be the amount we have left (Rebecka) 
	DamageToMake = FMath::Min(Health,DamageToMake);
	Health -= DamageToMake;
	UE_LOG(LogTemp, Warning, TEXT("Health left: %f"), Health);
	
	return DamageToMake;
}
bool ARushEnemyAI::GetIsLaunchingAnimation()
{
	return IsLaunchingAnimation;
}

