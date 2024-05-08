// Fill out your copyright notice in the Description page of Project Settings.


#include "AlternativeFireMode.h"
#include "SlimeProjectile.h"


void AAlternativeFireMode::BeginPlay()
{
	Super::BeginPlay();
	Slime = GetWorld()->SpawnActor<ASlimeProjectile>(Projectile, GetMuzzlePoint()->GetComponentLocation(), GetMuzzlePoint()->GetComponentRotation());
	
	Slime->SetActorHiddenInGame(true);
	Slime->SetActorEnableCollision(false);
	Slime->SetActorTickEnabled(false);

	UProjectileMovementComponent* TempMove = Slime->GetProjectileMovementComponent();
	TempMove->SetActive(false);
	Slime->SetProjectileMovementComponent(TempMove);
        
}


void AAlternativeFireMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bSlimeCharged && bButtonReleased)
	{
		bSlimeCharged = false;
		FireWeapon();
	}
}

void AAlternativeFireMode::InitiateTimer(bool bButtonPressed, bool bAlternative)
{
	if(bAlternative && SlimeAmmo > 0)
	{
		Super::PullTrigger(bButtonPressed, bAlternative, &Super::SlimeCharge, this);
		
	}
	else if(!bAlternative)
	{
		Super::PullTrigger(bButtonPressed, bAlternative, &ANonProjectileWeapon::Shoot, this);
	}
	
}


int32 AAlternativeFireMode::GetCurrentSlimeAmmo() const
{
	return SlimeAmmo;
}

FString AAlternativeFireMode::GetSlimeAmmo() const
{
	return FString::Printf(TEXT("%d"), SlimeAmmo);
}

void AAlternativeFireMode::FireWeapon()
{
	Slime->SetActorLocation(GetMuzzlePoint()->GetComponentLocation());
	Slime->SetActorRotation(GetMuzzlePoint()->GetComponentRotation());

	Slime->SetActorHiddenInGame(false);
	Slime->SetActorEnableCollision(true);
	Slime->SetActorTickEnabled(true);

	UProjectileMovementComponent* TempMove = Slime->GetProjectileMovementComponent();
	TempMove->SetActive(true);

	FVector ViewportCenter(GetLocation().X * 0.5f, GetLocation().Y * 0.5f, 0.0f);
	
	TempMove->AddForce((Super::GetLocation() + Super::GetRotation().Vector() * 900000));
	
	Slime->SetProjectileMovementComponent(TempMove);
	
	SlimeAmmo--;
	
}

int32 AAlternativeFireMode::SetSlimeAmmo(int32 _SlimeAmmo)
{
	return SlimeAmmo = _SlimeAmmo;
}
