// Fill out your copyright notice in the Description page of Project Settings.


#include "RushEnemyAI.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStaticsTypes.h"

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
	
	//För att Jeremy ska kunna hantera Death i sin EnemySpawn(Hanna)
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
	if(!bCanAttack)
	{
		return;
	}
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * MaxTraceDistance; 

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); 
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, TraceChannel, Params);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 0.1f, 0,2);
	if (bHit)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(HitResult.GetActor());
		if (Player)
		{
			DealDamageToPlayer();
			StartAttackDelay();
		}
	}
}
void ARushEnemyAI::DealDamageToPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy did damage!"));
}

void ARushEnemyAI::StartAttackDelay()
{
	if (!bCanAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack cooldown already active."));
		return;
	}

	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackCooldown, this, &ARushEnemyAI::EndAttackDelay, 3.0f, false);
	UE_LOG(LogTemp, Warning, TEXT("Attack cooldown started."));
}

void ARushEnemyAI::EndAttackDelay()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack cooldown ended."));
	bCanAttack = true;
}


