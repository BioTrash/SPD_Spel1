// Fill out your copyright notice in the Description page of Project Settings.

struct FDamageEvent;

#include "RushEnemyAI.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/DamageType.h"

//Hanna
ARushEnemyAI::ARushEnemyAI()
{
	//Konstruktor, sätter bool till falskt och sätter igång tick
	PrimaryActorTick.bCanEverTick = true;
	bHasExploded = false;
}
//Körs i början av spelet, sätter health till maxhealth
void ARushEnemyAI::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}
//hanterar fiendes beteende och explosion, så om hälsan är under 0 ska den explodera om inte kallar den på performlinetrace
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
//Spelarinputkomponent
void ARushEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
//Utför själva explosionen, om spelaren är inom radiusen kommer den att skada av explosionen
void ARushEnemyAI::Explode(float Damage, bool bCollisionTriggered)
{
	if (bCollisionTriggered && !bHasExploded)
	{
		TArray<AActor*> OverlappingActors;
		FVector ExplosionLocation = GetActorLocation();

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), OverlappingActors);

		for (AActor* Actor : OverlappingActors)
		{
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(Actor))
			{
				float DistanceToPlayer = FVector::Distance(ExplosionLocation, Player->GetActorLocation());
				float DamageMultiplier = FMath::Clamp(1.0f - (DistanceToPlayer / DamageRadius), 0.0f, 1.0f);
				float ActualDamage = Damage * DamageMultiplier;

				Player->TakeDamage(ActualDamage, FDamageEvent(), GetController(), this);
			}
		}
	//Effekten för explosionen
	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, ExplosionLocation, FRotator::ZeroRotator, FVector::OneVector, true, true);
	}
		bHasExploded = true;
		OnEnemyDeath();
		Destroy();
	}
}
//Utför linetrace för att se om fienden har kolliderat med spelaren
void ARushEnemyAI::PerformLineTrace()
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * MaxTraceDistance; 

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); 
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Pawn, Params))
	{
		Explode(30, true);
	}
}
//Ser till att spelaren tar skada
float ARushEnemyAI::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToMake = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToMake = FMath::Min(Health,DamageToMake);
	Health -= DamageToMake;
	if (Health <= 0 && !bHasExploded)
	{
		Explode(30.f, true);
	}
	return DamageToMake;
}
//För Noras animationer
bool ARushEnemyAI::GetIsLaunchingAnimation()
{
	return IsLaunchingAnimation;
}

