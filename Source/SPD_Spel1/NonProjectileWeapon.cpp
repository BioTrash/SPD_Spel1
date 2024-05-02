// Fill out your copyright notice in the Description page of Project Settings.


#include "NonProjectileWeapon.h"

#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"

ANonProjectileWeapon::ANonProjectileWeapon() 
{
	// Root and Mesh are created and attached in the WeaponBase (Rufus)
}

void ANonProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	End = Super::GetLocation() + Super::GetRotation().Vector() * MaxShootingRange;

}

void ANonProjectileWeapon::InitiateTimer(bool bButtonHeld)
{
	Super::PullTrigger(bButtonHeld, &ANonProjectileWeapon::Shoot, this);
}


void ANonProjectileWeapon::Shoot()
{

	// Is needed because direct shots get blocked by colliders at spawn, potentially damaging the actor that shot (Rufus)
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // Ignores the weapon collision (Rufus)
	Params.AddIgnoredActor(GetOwner()); // Ignores the actor that shot (Rufus)
	//Params.AddIgnoredActor(Cast<AActor>(Projectile)); // Ignores the same projectiles (Rufus)
	
	//Makes it so when the player shoots at an enemy it does extra damage if it hits it SphereComponent which is on the enemies head(Hanna)
	//Basically a headshot implementation(Hanna)
	if(GetWorld()->LineTraceSingleByChannel(Hit, Super::GetLocation(), End, ECC_GameTraceChannel2, Params))
	{
		AActor* HitActor = Hit.GetActor();
		TArray<USphereComponent*> SphereComponents;
		HitActor->GetComponents<USphereComponent>(SphereComponents);

		for (USphereComponent* SphereComponent : SphereComponents)
		{
			if (SphereComponent->ComponentHasTag("Headshot"))
			{
					FVector SphereCenter = SphereComponent->GetComponentLocation();
					FVector HitToCenter = Hit.Location - SphereCenter;
					//DrawDebugLine(GetWorld(), SphereCenter, Hit.Location, FColor::Blue, false, 1.0f, 0, 1.0f);
				
				if (HitToCenter.SizeSquared() <= FMath::Square(SphereComponent->GetScaledSphereRadius()))
				{
				FString ComponentName = SphereComponent->GetName();
				Damage += 20.0f;
				UE_LOG(LogTemp, Warning, TEXT("Headshot works: %s"), *ComponentName);
				DrawDebugSphere(GetWorld(), Hit.Location, 20.0f, 32, FColor::Green, false, 1.0f);
				}
			}
		}
		//I did this >:) (Hanna)
	
	
	
		if(Super::GetCurrentClip() > 0)
		{
			//FMath::RandRange(int32 min, int32 max);
			DrawDebugPoint(GetWorld(), Hit.Location, 20, FColor::Red, false, 1.0f);

			//NEW CHANGES; CAN REMOVE IF NOT WORKING
 			AActor* HitActorHeadshot = Hit.GetActor();
 			//if we hit an actor, we make the actor take damage (Rebecka)
 			if(HitActorHeadshot != nullptr)
 			{
 				FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
 				HitActorHeadshot->TakeDamage(Damage, DamageEvent, Super::GetOwnerController(), this);
 				Super::WhenShot();
 			}

	
			Super::SetCurrentClip(Super::GetCurrentClip()-1);
			
 			
 			if(GetCurrentClip() == 0)
 			{
 				Super::Reload();
 			}
 		}
 		else
 		{
 			Super::Reload();
 		}
 	}
}

FVector ANonProjectileWeapon::GetEnd() const
{
	return End;
}

FVector ANonProjectileWeapon::GetShotDirection() const
{
	return ShotDirection;
}

FHitResult ANonProjectileWeapon::GetHitResult() const
{
	return Hit;
}

float ANonProjectileWeapon::GetDamage() const
{
	return Damage;
}

float ANonProjectileWeapon::GetMaxShootingRange() const
{
	return MaxShootingRange;
}

void ANonProjectileWeapon::SetMaxShootingRange(float NewMaxShootingRange)
{
	MaxShootingRange = NewMaxShootingRange;
}

void ANonProjectileWeapon::SetDamage(float NewDamage)
{
	Damage = NewDamage;
}



