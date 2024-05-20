// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NonProjectileWeapon.h"
#include "AlternativeFireMode.generated.h"

/**
 * 
 */
UCLASS()
class SPD_SPEL1_API AAlternativeFireMode : public ANonProjectileWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void InitiateTimer(bool bButtonPressed, bool bAlternative) override;

	void FireWeapon();

	UFUNCTION(BlueprintPure)
	int32 SetSlimeAmmo(int32 _SlimeAmmo);
	
	UFUNCTION(BlueprintPure)
	int32 GetCurrentSlimeAmmo() const;
	UFUNCTION(BlueprintPure)
	FString GetSlimeAmmo() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SlimeAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxSlimeAmmo;

	UPROPERTY(EditAnywhere, Category="Weaponry")
	TSubclassOf<class ASlimeProjectile> Projectile;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ASlimeProjectile* Slime;
	

private:
	bool IsWeaponCharging;
	
	float CurrentChargeTime;
	
};
