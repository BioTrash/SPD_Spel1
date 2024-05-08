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
	
}

// Called every frame
void ASlimeBossAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASlimeBossAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ASlimeBossAI::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 1;
}
void ASlimeBossAI::KillEnemy()
{
	OnEnemyDeath();
}
