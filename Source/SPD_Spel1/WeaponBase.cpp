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

}

template <typename T>
void AWeaponBase::PullTrigger(bool bRapidFire, T& Func)
{
	if(bRapidFire)
	{
		GetWorld()->GetTimerManager().SetTimer(RapidFireTimer, this, Func, 0.1f, true, 0.0f);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(RapidFireTimer);
	}
}



