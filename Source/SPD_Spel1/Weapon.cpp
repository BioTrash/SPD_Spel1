// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "PlayerCharacter.h"
#include "Projectile.h"
#include "Camera/CameraComponent.h"


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
	
	// Is needed in order to get controller (Rufus)
	OwnerCharacter = Cast<APawn>(GetOwner());
	if(!OwnerCharacter) return;

	// Is needed in order to get PlayerViewPort (Rufus)
	OwnerController = OwnerCharacter->GetController();
	if(!OwnerController) return;

	// Is needed in order to establish max range and direction for directs shots, i.e. non-projectile (Rufus)
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	if(Recoil)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
		if(!Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter not found or conversion failed"));
			return;
		}
	
		
		PlayerCamera = Player->GetFPSCameraComponent();
		if(!PlayerCamera)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerCamera not found"));
			return;
		}
	}

	if (Spread)
	{
		// Calculate spread for all axes
		FVector SpreadAmount(
			FMath::RandRange(-SpreadSize, SpreadSize),   // Spread along X
			FMath::RandRange(-SpreadSize, SpreadSize),   // Spread along Y
			FMath::RandRange(-SpreadSize, SpreadSize)    // Spread along Z
		);

		// Apply spread to the location
		FVector SpreadLocation = Location + SpreadAmount;

		// Calculate the endpoint after applying spread
		End = SpreadLocation + Rotation.Vector() * MaxShootingRange;
	}
	else
	{
		// End is max range (Rufus)
		End = Location + Rotation.Vector() * MaxShootingRange;
	}
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
			//PlayerCamera->SetWorldRotation(FQuat(FRotator(PlayerCamera->GetComponentRotation().Pitch, PlayerCamera->GetComponentRotation().Yaw, PlayerCamera->GetComponentRotation().Roll)));
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
	
	// If this suddenly stops working or direct shots get stuck in air check whether the ECC_GameTraceChannel is still correct (Rufus)
	// Messing around with order in 'Project Settings -> Engine -> Collision -> Trace Channels' may break it (Rufus)
	// Check current GameTraceChanel by going 'SPD_Spel1\Config\GameEngine.ini' and searching for the name of the channel in question as written in Project Settings. (Rufus)
	if(GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_GameTraceChannel2, Params))
	{
		if(UnlimitedAmmo || CurrentClip > 0)
		{
			DrawDebugPoint(GetWorld(), Hit.Location, 20, FColor::Red, false, 1.0f);


			// TIP FOR TMRWS BUG FIX: CAMERA'S PITCH DOESN'T LISTEN TO PLAYER INPUTS WHILE SHOOTING
			// POSSIBLE SOLUTION: CURRENT PITCH += RECOILCAMERAOFFSET
			
			if(PlayerCamera)
			{
				// FQuat NewCameraQuat = PlayerCamera->GetComponentQuat();
				// FQuat RecoilQuat = FQuat(FRotator(Rotation.Pitch, 0.0f, 0.0f));
				// NewCameraQuat *= RecoilQuat;
				// NewCameraQuat.Normalize();

				float CurrentPitch = PlayerCamera->GetComponentRotation().Pitch;
				PlayerCamera->SetWorldRotation(FRotator(CurrentPitch += RecoilCameraOffset, PlayerCamera->GetComponentRotation().Yaw, PlayerCamera->GetComponentRotation().Roll));
			}
			
			//NEW CHANGES; CAN REMOVE IF NOT WORKING
			AActor* HitActor = Hit.GetActor();
			//if we hit an actor, we make the actor take damage (Rebecka)
			if(HitActor != nullptr)
			{
				FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
				HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
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















