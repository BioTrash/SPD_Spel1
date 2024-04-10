// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class SPD_SPEL1_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Is a shoot function, triggered from PlayerCharacter.cpp under Shoot function as well as CancelShoot under it (Rufus)
	void PullTrigger(bool SprayShooting);
	
private:
	UPROPERTY(EditDefaultsOnly)
	bool UnlimitedAmmo = false;
	UPROPERTY(EditDefaultsOnly)
	int32 TotalAmmo;
	UPROPERTY(EditDefaultsOnly)
	int32 CurrentClip;
	UPROPERTY(EditDefaultsOnly)
	int32 ClipSize;
	
	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotation;
	UPROPERTY()
	FVector End;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class USplineMeshComponent* Spline;

	UPROPERTY()
	AController* OwnerController;
	
	// Max range is applicable only when no projectile is found, in case of projectile max range should be regulated with physics (Rufus)
	UPROPERTY(EditAnywhere, Category="Weaponry")
	float MaxShootingRange = 1000;

	// If no projectile is set in BlueprintEditor the fallback is direct shots
	UPROPERTY(EditAnywhere, Category="Weaponry")
	TSubclassOf<class AProjectile> Projectile;

	FTimerHandle SprayShootingTimer;

	// The default way of shooting if no projectile is specified in blueprints (Rufus)
	void ShootWithoutProjectile();

	// Shooting method called if a projectile is specified in blueprints (Rufus)
	void ShootProjectile();

	UPROPERTY(EditAnywhere)
	float Damage = 10;
	
};
