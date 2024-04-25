// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

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

void AWeaponBase::Reload()
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
}


FString AWeaponBase::GetAmmo() const
{
	return FString::Printf(TEXT("%d / %d"), TotalAmmo, CurrentClip);
}


