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
	virtual void BeginPlay() override;
	
	ANonProjectileWeapon();
	virtual void Shoot() override;
	virtual void InitiateTimer(bool bButtonPressed, bool bAlternative) override;

	UFUNCTION(BlueprintPure)
	FVector GetEnd() const;
	UFUNCTION(BlueprintPure)
	FVector GetShotDirection() const;
	UFUNCTION(BlueprintPure)
	FHitResult GetHitResult() const;
	UFUNCTION(BlueprintPure)
	float GetDamage() const;
	UFUNCTION(BlueprintPure)
	float GetMaxShootingRange() const;

	UFUNCTION(BlueprintCallable)
	void SetDamage(float NewDamage);
	UFUNCTION(BlueprintCallable)
	void SetMaxShootingRange(float NewMaxShootingRange);
protected:
	UPROPERTY(EditAnywhere)
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 30.0f;

	UPROPERTY()
	FHitResult Hit;

	FCollisionQueryParams Params;

	UPROPERTY()
	FVector End;
private:

	UPROPERTY()
	FVector ShotDirection;

	float OriginalDamage = Damage;
	
	UPROPERTY(EditAnywhere)
	float MaxShootingRange = 100000;
	
};
