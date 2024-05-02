// Fill out your copyright notice in the Description page of Project Settings.

struct FDamageEvent;

#include "RushEnemyAI.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"

ARushEnemyAI::ARushEnemyAI()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ARushEnemyAI::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

void ARushEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PerformLineTrace();
	if(Health <= 0)
	{
		KillEnemy();
	}
}

void ARushEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARushEnemyAI::KillEnemy()
{
	UE_LOG(LogTemp, Warning, TEXT("ENEMY SHOULD DIE"));

	OnEnemyDeathDelegate.Broadcast();

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

void ARushEnemyAI::PerformLineTrace()
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * MaxTraceDistance; 

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); 
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, TraceChannel, Params);
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 0.1f, 0,2);
	if (bHit)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(HitResult.GetActor());
		if (Player && bCanAttack)
		{
			DealDamageToPlayer(5.0f);
			bCanAttack = false;
			GetWorldTimerManager().SetTimer(ExplodeCooldown, this, &ARushEnemyAI::Explode, 3.0f, false);
		}
	}
	}

void ARushEnemyAI::DealDamageToPlayer(float Damage)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
		float DistanceMultiplier = FMath::Clamp(1.0f - (DistanceToPlayer / DamageRadius), 0.0f, 1.0f);
		float ActualDamage = PlayerCharacter->TakeDamage(Damage * DistanceMultiplier, FDamageEvent(), GetController(), this);

		UE_LOG(LogTemp, Warning, TEXT("Damage done: %f"), ActualDamage);
	}
	UE_LOG(LogTemp, Warning, TEXT("Enemy did damage!"));
	DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 32, FColor::Red, false, 2.0f);
}

void ARushEnemyAI::Explode()
{
	DealDamageToPlayer(30.0f);
	KillEnemy();
}
void ARushEnemyAI::EndExplodeCooldown()
{
	bCanAttack = true;
}






