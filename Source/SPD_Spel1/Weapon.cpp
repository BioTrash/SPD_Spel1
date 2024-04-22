// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Projectile.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ActorComponent.h"


// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	Mesh->SetupAttachment(Root);


	// This part is currently missing a projectile spawn point. Ignored because we lack the actual weapon meshes (Rufus)
	
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// End is max range (Rufus)
	End = Location + Rotation.Vector() * MaxShootingRange;

	// Is needed in order to get controller (Rufus)
	APawn* OwnerCharacter = Cast<APawn>(GetOwner());
	if(!OwnerCharacter) return;

	// Is needed in order to get PlayerViewPort (Rufus)
	OwnerController = OwnerCharacter->GetController();
	if(!OwnerController) return;

	// Is needed in order to establish max range and direction for directs shots, i.e. non-projectile (Rufus)
	OwnerController->GetPlayerViewPoint(Location, Rotation);

}

void AWeapon::PullTrigger(bool SprayShooting)
{
	//UE_LOG(LogTemp, Warning, TEXT("Trigger has been pulled"));
	
	if(Projectile)
	{
		if(SprayShooting)
		{
			GetWorld()->GetTimerManager().SetTimer(SprayShootingTimer, this, &AWeapon::ShootProjectile, 0.1f, true, 0.0f);
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(SprayShootingTimer);
		}
		
	}
	else
	{
		if(SprayShooting)
		{
			GetWorld()->GetTimerManager().SetTimer(SprayShootingTimer, this, &AWeapon::ShootWithoutProjectile, 0.1f, true, 0.0f);			
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(SprayShootingTimer);
		}
	}
}

void AWeapon::ShootWithoutProjectile()
{
	FVector ShotDirection;
	FHitResult Hit;

	// Is needed because direct shots get blocked by colliders at spawn, potentially damaging the actor that shot (Rufus)
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // Ignores the weapon collision (Rufus)
	Params.AddIgnoredActor(GetOwner()); // Ignores the actor that shot (Rufus)
	//Params.AddIgnoredActor(Cast<AActor>(Projectile)); // Ignores the same projectiles (Rufus)
	
	//Makes it so when the player shoots at an enenmy it does extra damage if it hits it SphereComponent which is on the enemies head(Hanna)
	//Basically a headshot implementation(Hanna)
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
		
		
		
		if(UnlimitedAmmo || CurrentClip > 0)
		{
			//FMath::RandRange(int32 min, int32 max);
			DrawDebugPoint(GetWorld(), Hit.Location, 20, FColor::Red, false, 1.0f);

			//NEW CHANGES; CAN REMOVE IF NOT WORKING
			AActor* HitActorHeadshot = Hit.GetActor();
			//if we hit an actor, we make the actor take damage (Rebecka)
			if(HitActorHeadshot != nullptr)
			{
				FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
				HitActorHeadshot->TakeDamage(Damage, DamageEvent, OwnerController, this);
			}
			
			CurrentClip--;
			
			if(CurrentClip == 0)
			{
				Reload();
			}
		}
		else
		{
			Reload();
		}

	}
}
}

void AWeapon::ShootProjectile()
{
	// ForwardVector is not required but is a workaround until spawn point of real meshes is set (Rufus)
	// CHANGE WHEN WEAPON MESHES ARE MADE! (Rufus)
	GetWorld()->SpawnActor<AProjectile>(Projectile, Location + GetOwner()->GetActorForwardVector()*100, Rotation);
}

void AWeapon::Reload()
{
	if(UnlimitedAmmo)
	{
		CurrentClip = ClipSize;
	}
	else
	{
		if(TotalAmmo-ClipSize > 0)
		{
			TotalAmmo -= (ClipSize - CurrentClip);
			CurrentClip = ClipSize;
		}
		else
		{
			CurrentClip = TotalAmmo;
			TotalAmmo = 0;
		}
	}


	UE_LOG(LogTemp, Warning, TEXT("Total Ammo: %d, CurrentAmmo: %d"), TotalAmmo, CurrentClip);
}

FString AWeapon::GetAmmo() const
{
	return FString::Printf(TEXT("%d / %d"), TotalAmmo, CurrentClip);
}