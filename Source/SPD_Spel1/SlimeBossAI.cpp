// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeBossAI.h"

// Sets default values
ASlimeBossAI::ASlimeBossAI()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASlimeBossAI::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

// Called every frame
void ASlimeBossAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(Health <= 0)
	{
		KillEnemy();
	}
}

// Called to bind functionality to input
void ASlimeBossAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ASlimeBossAI::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToMake = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//to make sure that the DamageToMake is not greater than the health we have left, therefore we make the DamageToMake to be the amount we have left (Rebecka) 
	DamageToMake = FMath::Min(Health,DamageToMake);
	Health -= DamageToMake;
	UE_LOG(LogTemp, Warning, TEXT("Health left: %f"), Health);
	return DamageToMake;
}
void ASlimeBossAI::KillEnemy()
{
	UE_LOG(LogTemp, Warning, TEXT("BOSSEN DOG!!"));
	OnEnemyDeath();
	Destroy();
}
