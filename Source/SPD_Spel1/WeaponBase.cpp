// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(Root);

}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponBase::Tick(float DeltaTime)
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

}

void AWeaponBase::DelaySwitch()
{
	bDelayed = true;
}

void AWeaponBase::SlimeCharge()
{
	bSlimeCharged = true;
	UE_LOG(LogTemp, Error, TEXT("SLIME CHARGED"));
}


bool AWeaponBase::GetbUnlimitedAmmo() const
{
	return bUnlimitedAmmo;
}

int32 AWeaponBase::GetTotalAmmo() const
{
	return TotalAmmo;
}

int32 AWeaponBase::GetCurrentClip() const
{
	return CurrentClip;
}

int32 AWeaponBase::GetClipSize() const
{
	return ClipSize;
}

AController* AWeaponBase::GetOwnerController() const
{
	return OwnerController;
}

APawn* AWeaponBase::GetOwnerCharacter() const
{
	return OwnerCharacter;
}

FVector AWeaponBase::GetLocation() const
{
	return Location;
}

FRotator AWeaponBase::GetRotation() const
{
	return Rotation;
}

USceneComponent* AWeaponBase::GetMuzzlePoint() const
{
	return MuzzlePoint;
}


void AWeaponBase::SetbUnlimitedAmmo(bool _bUnlimitedAmmo) {
	bUnlimitedAmmo = _bUnlimitedAmmo;
}

void AWeaponBase::SetTotalAmmo(int32 _TotalAmmo) {
	TotalAmmo = _TotalAmmo;
}

void AWeaponBase::SetCurrentClip(int32 _CurrentClip) {
	CurrentClip = _CurrentClip;
}

void AWeaponBase::SetClipSize(int32 _ClipSize) {
	ClipSize = _ClipSize;
}

void AWeaponBase::SetFireRate(int32 _FireRate)
{
	FireRate = _FireRate;
}


void AWeaponBase::InitiateReload()
{
	if(bUnlimitedAmmo)
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

	bReloading = false;
	bDelayed = true;
}


FString AWeaponBase::GetAmmo() const
{
	return FString::Printf(TEXT("%d / ∞"), CurrentClip);
}



