// Fill out your copyright notice in the Description page of Project Settings.


#include "NonProjectileWeapon.h"

#include "Components/SphereComponent.h"

ANonProjectileWeapon::ANonProjectileWeapon() 
{
	// Root and Mesh are created and attached in the WeaponBase (Rufus)
}

void ANonProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Is needed in order to get controller (Rufus)
	OwnerCharacter = Cast<APawn>(GetOwner());
	if(!OwnerCharacter) return;

	// Is needed in order to get PlayerViewPort (Rufus)
	OwnerController = OwnerCharacter->GetController();
	if(!OwnerController) return;

	// Is needed in order to establish max range and direction for directs shots, i.e. non-projectile (Rufus)
	OwnerController->GetPlayerViewPoint(Location, Rotation);

	End = Location + Rotation.Vector() * 10000;

}

void ANonProjectileWeapon::InitiateTimer(bool bButtonHeld)
{
	Super::PullTrigger(bButtonHeld, &ANonProjectileWeapon::Shoot, this);
}


void ANonProjectileWeapon::Shoot()
{
	FVector ShotDirection;
	FHitResult Hit;

	// Is needed because direct shots get blocked by colliders at spawn, potentially damaging the actor that shot (Rufus)
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // Ignores the weapon collision (Rufus)
	Params.AddIgnoredActor(GetOwner()); // Ignores the actor that shot (Rufus)
	//Params.AddIgnoredActor(Cast<AActor>(Projectile)); // Ignores the same projectiles (Rufus)
	
	// If this suddenly stops working or direct shots get stuck in air check whether the ECC_GameTraceChannel is still correct (Rufus)
	// Messing around with order in 'Project Settings -> Engine -> Collision -> Trace Channels' may break it (Rufus)
	// Check current GameTraceChanel by going 'SPD_Spel1\Config\GameEngine.ini' and searching for the name of the channel in question as written in Project Settings. (Rufus)
	if(GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_GameTraceChannel2, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->ActorHasTag("Enemy"))
		{
			TArray<USphereComponent*> SphereComponents;
			HitActor->GetComponents<USphereComponent>(SphereComponents);

			for (USphereComponent* SphereComponent : SphereComponents)
			{
				if (SphereComponent->ComponentHasTag("Headshot"))
				{
					FString ComponentName = SphereComponent->GetName();
					//Damage += 20.0f;
					UE_LOG(LogTemp, Warning, TEXT("Headshot works: %s"), *ComponentName);
					DrawDebugSphere(GetWorld(), Hit.Location, 20.0f, 32, FColor::Green, false, 1.0f);
				}
			}
		}
	}
}

