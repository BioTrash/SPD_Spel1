// Fill out your copyright notice in the Description page of Project Settings.


#include "AlternativeFireMode.h"
#include "SlimeProjectile.h"


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
	
	//GetWorld()->SpawnActor<ASlimeProjectile>(Projectile, GetLocation() + GetOwner()->GetActorForwardVector()* 100, GetRotation());
	GetWorld()->SpawnActor<ASlimeProjectile>(Projectile, GetMuzzlePoint()->GetComponentLocation(), GetMuzzlePoint()->GetComponentRotation());
	SlimeAmmo--;
}

int32 AAlternativeFireMode::SetSlimeAmmo(int32 _SlimeAmmo)
{
	return SlimeAmmo = _SlimeAmmo;
}
