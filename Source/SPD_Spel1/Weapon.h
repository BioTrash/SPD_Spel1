// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
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

	void Reload();

	UFUNCTION(BlueprintPure)
	FString GetAmmo() const;
	
private:
	UPROPERTY(EditAnywhere)
	bool UnlimitedAmmo = false;
	UPROPERTY(EditAnywhere)
	bool Spread = false;
	UPROPERTY(EditAnywhere)
	bool Recoil = false;
	UPROPERTY(EditAnywhere)
	bool RecoilRecovery = false;
	UPROPERTY(EditAnywhere)
	int32 TotalAmmo;
	UPROPERTY(EditAnywhere)
	int32 CurrentClip;
	UPROPERTY(EditAnywhere)
	int32 ClipSize;
	UPROPERTY(EditAnywhere)
	int32 SpreadSize;
	UPROPERTY(EditAnywhere)
	int32 RecoilPower;
	
	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotation;
	UPROPERTY()
	FVector End;
	UPROPERTY()
	FRotator DefaultRotation;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY()
	AController* OwnerController;

	UPROPERTY()
	APawn* OwnerCharacter;

	UPROPERTY()
	class UCameraComponent* PlayerCamera;

	UPROPERTY()
	APlayerCameraManager* CameraManager;

	// RecoilOffset get reset when SprayShootingTimer resets (Rufus)
	UPROPERTY(EditAnywhere, Category="Weaponry")
	float RecoilCameraOffset = 1.0f;

	UPROPERTY(EditAnywhere, Category="Weaponry")
	float RecoilRecoverySpeed = 5.0f;

	UPROPERTY()
	float TotalRecoil = 0.0f;
	
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
