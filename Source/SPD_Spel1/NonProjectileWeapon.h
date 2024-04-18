// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "NonProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SPD_SPEL1_API ANonProjectileWeapon : public AWeaponBase
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;
	
	ANonProjectileWeapon();
	virtual void Shoot() override;
	
private:
	UPROPERTY()
	AController* OwnerController;

	UPROPERTY()
	APawn* OwnerCharacter;

	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotation;
	UPROPERTY()
	FVector End;
	
};
