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
	if(IsShielded())
	{
		return 0.0f;
	}
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    ActualDamage = FMath::Min(Health, ActualDamage);
    Health -= ActualDamage;
    return ActualDamage;
}
float ASlimeBossAI::GetHealth() const
{
    return Health; 
}

void ASlimeBossAI::ResetHealth()
{
	Health = MaxHealth;
}

float ASlimeBossAI::GetHealthPercentage() const
{
	return Health / MaxHealth;
}

void ASlimeBossAI::KillEnemy()
{
	//UE_LOG(LogTemp, Warning, TEXT("BOSSEN DOG!!"));
	OnEnemyDeath();
	Destroy();
}
//Aktiverar och inaktiverar skölden som bossen har, används för slam också för iframes
void ASlimeBossAI::SetShield(bool bShielded)
{
	//Sätter skölden til det angivna värdet
	bShield = bShielded;
	//Om skölden är aktiverad
	if(bShield)
	{
		//Sätter igång en timer för att inaktivera den efter 2 sekunder
		GetWorldTimerManager().SetTimer(ShieldTimerHandle, this, &ASlimeBossAI::DisableShield, 2.0f, false);
	}
	else //Inte är aktiverad
	{
		//Rensar timern för att undvika att skölden inaktiveras
		GetWorldTimerManager().ClearTimer(ShieldTimerHandle);
	}
}
//Returnerar true om bossen är sköldad annars false
//Hanna
bool ASlimeBossAI::IsShielded() const
{
	return bShield;
}
//Inaktiverar skölden för bossen
//Hanna
void ASlimeBossAI::DisableShield()
{
	bShield = false;
}
bool ASlimeBossAI::GetIsSlammingAnimation()
{
	return IsSlammingAnimation;
}



