// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyTurret.generated.h"

UCLASS()
class SPD_SPEL1_API AEnemyTurret : public APawn
{
	GENERATED_BODY()

public:
	AEnemyTurret();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnEnemyDeath();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
    void OnShooting();
	
	UFUNCTION(BlueprintCallable, Category="Enemy")
	void ShootEnemy(float Damage);
	
	UFUNCTION(BlueprintCallable, Category="Animations")
	bool GetIsShootingAnimation();

	UFUNCTION(BlueprintCallable, Category="Animations")
	void SetIsShootingAnimation(bool _IsShootingAnimation);

	bool IsShootingAnimation;
	
	// Jeremy :)
	UFUNCTION(BlueprintCallable)
	void ResetHealth();
	
private:
	void InitializeTurretComponents();
	void RotateTurret(FVector TargetLocation);
	void ShootProjectiles();
	void HandleDeath();
	void ShootAgainCooldown();

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 30.f;
	
	UPROPERTY(EditDefaultsOnly)
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, Category="Turret")
	class APlayerCharacter* Player;
	
	UPROPERTY(VisibleAnywhere, Blueprintable, Category="Components")
	class UCapsuleComponent* CapsuleComponent;
	
	UPROPERTY(VisibleAnywhere, Blueprintable, Category="Components")
	UStaticMeshComponent* BaseMesh;
	
	UPROPERTY(VisibleAnywhere, Blueprintable, Category="Components")
	USkeletalMeshComponent* TurretMesh;
	
	UPROPERTY(VisibleAnywhere, Blueprintable, Category="Components")
	USceneComponent* ProjectileSpawn;
	
	UPROPERTY(EditAnywhere, Category="Turret")
	float FireRange = 1200.f;

	UPROPERTY(EditAnywhere, Blueprintable, Category="Combat")
	float RotationSpeed = 5.f;
	
	float MaxTraceDistance = 1000.f;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_GameTraceChannel1;

	float ProjectileSpawnCooldown = 1.0f; 
	float NextProjectileTime = 0.0f;
	float NextShootTime = 0.f;
	float ShootCooldown = 4.f;
	float DamageRadius = 10.0f;
};
