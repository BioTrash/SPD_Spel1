// Fill out your copyright notice in the Description page of Project Settings.

#include "RushEnemyAI.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/DamageType.h"
#include "Engine/DamageEvents.h"

//Hanna
ARushEnemyAI::ARushEnemyAI()
	:	IsLaunchingAnimation(false),
		bHasExploded(false),
		Health(0.0f),
		ExplosionEffect(nullptr)
{
	//Konstruktor, sätter bool till falskt och sätter igång tick
	PrimaryActorTick.bCanEverTick = true;
	//Sätter boolen till false
	bHasExploded = false;
}
//Körs i början av spelet, sätter health till maxhealth
void ARushEnemyAI::BeginPlay()
{
	Super::BeginPlay();
	//Sätter Health till MaxHealth
	Health = MaxHealth;
}
//hanterar fiendes beteende och explosion, så om hälsan är under 0 ska den explodera om inte kallar den på performlinetrace
void ARushEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Om hälsan är större än 0 kallar den på performlinetrace metoden
	if(Health > 0)
	{
		PerformLineTrace();
	}
	//Om hälsan är mindre eller lika med 0 och fienden inte har exploderar
	if(Health <= 0 && !bHasExploded)
	{
		//Utför själva explosionen med 30 skada och sätter bHasExploded till true
		Explode(50.f, true);
		bHasExploded = true;
	}
}
//Utför själva explosionen, skadar spelaren om den är inom radius
void ARushEnemyAI::Explode(float Damage, bool bCollisionTriggered)
{
	//Kollar om explosionen triggas av en kollision och om fienden inte redan har exploderat
	if (bCollisionTriggered && !bHasExploded)
	{
		//Lista över överlappande aktörer
		TArray<AActor*> OverlappingActors;
		//Hämtar explosionens position
		FVector ExplosionLocation = GetActorLocation();
		//Hämtar alla aktörer av APlayerCharacter som finns i världen
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), OverlappingActors);

		//Loopar igenom alla overlappingactors
		for (AActor* Actor : OverlappingActors)
		{
			//Kollar om aktören är en spelare
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(Actor))
			{
				//Beröknar avståndet som finns till spelaren
				float DistanceToPlayer = FVector::Distance(ExplosionLocation, Player->GetActorLocation());
				//Beröknar DamageMultiplier baserat på avståndet
				float DamageMultiplier = FMath::Clamp(1.0f - (DistanceToPlayer / DamageRadius), 0.0f, 1.0f);
				//Beräknar sedan den faktiska skadan
				float ActualDamage = Damage * DamageMultiplier;
				//Gör att spelaren kan ta skada
				Player->TakeDamage(ActualDamage, FDamageEvent(), GetController(), this);
			}
		}
	//Effekten för explosionen, kollar om den är satt
	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, ExplosionLocation, FRotator::ZeroRotator, FVector::OneVector, true, true);
	}
		//Sätter Bool till true och anropar OnEnemyDeath() och destroyar fienden
		bHasExploded = true;
		OnEnemyDeath();
		Destroy();
	}
}
//Utför linetrace för att se om fienden har kolliderat med spelaren
void ARushEnemyAI::PerformLineTrace()
{
	// Sätter startpositionen för linetrace är fiendes position
	FVector StartLocation = GetActorLocation();
	//Slutpositionen är en position framför fienden
	FVector EndLocation = StartLocation + GetActorForwardVector() * MaxTraceDistance; 

	//Resultatet av linetrace
	FHitResult HitResult;
	//Kollideringsparameter
	FCollisionQueryParams Params;
	//Ignorerar sig själv vid linetrace
	Params.AddIgnoredActor(this); 
	// Utgör själva linetracen och kontroller om den träffat något
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Pawn, Params))
	{
		// Kontrollera om det träffade objektet är spelaren
		APlayerCharacter* Player = Cast<APlayerCharacter>(HitResult.GetActor());
		if (Player)
		{
			// Om den då träffat spelaren kallar den på Explode funktionen
			Explode(50, true);
		}
	}
}
//Ser till att spelaren tar skada(Rebecka)
float ARushEnemyAI::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToMake = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToMake = FMath::Min(Health,DamageToMake);
	Health -= DamageToMake;

	//Kontrollerar om dens hälsa är mindre eller lika med 0 och fienden inte har exploderat(Hanna)
	if (Health <= 0 && !bHasExploded)
	{
		//Utför explosionen
		Explode(50.f, true);
	}
	return DamageToMake;
}
//För Noras animationer
bool ARushEnemyAI::GetIsLaunchingAnimation()
{
	return IsLaunchingAnimation;
}

