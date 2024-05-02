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











