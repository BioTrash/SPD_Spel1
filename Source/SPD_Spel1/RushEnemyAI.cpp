// Fill out your copyright notice in the Description page of Project Settings.


#include "RushEnemyAI.h"
#include "PlayerCharacter.h"
#include "PlayerCharacter.h"

// Sets default values
ARushEnemyAI::ARushEnemyAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARushEnemyAI::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

// Called every frame
void ARushEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void ARushEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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

void ARushEnemyAI::KillEnemy()
{
	UE_LOG(LogTemp, Warning, TEXT("ENEMY SHOULD DIE"));
	Destroy();
}




























