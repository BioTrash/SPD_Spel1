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
	// Sets default values for this pawn's properties
	AEnemyTurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnEnemyDeath();

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 40.f;
	
	UPROPERTY(VisibleAnywhere)
	float Health;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageRadius = 10.0f;
	
	UFUNCTION(BlueprintCallable, Category="Enemy")
	void ShootEnemy(float Damage);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	void Die();
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;
	
	float MaxTraceDistance = 300.f;
	
	UPROPERTY(VisibleAnywhere, Blueprintable, Category="Components")
	USkeletalMeshComponent* TurretMesh;

	bool IsShootingAnimation;

	UFUNCTION(BlueprintCallable, Category="Animations")
	bool GetIsShootingAnimation();
	
private:

	UPROPERTY(VisibleAnywhere, Blueprintable, Category="Components")
	class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(VisibleAnywhere, Blueprintable, Category="Components")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, Blueprintable, Category="Components")
	USceneComponent* ProjectileSpawn;
	
	UPROPERTY(EditAnywhere, Category="Turret")
	float FireRange = 1000.f;

	UPROPERTY(EditAnywhere, Category="Turret")
	class APlayerCharacter* Player;

	UPROPERTY(EditAnywhere, Blueprintable, Category="Combat")
	float RotationSpeed = 5.f;

	float NextShootTime = 0.f;
	float ShootCooldown = 4.f;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_GameTraceChannel1;

	float ProjectileSpawnCooldown = 4.0f; 
	float NextProjectileTime = 0.0f;
	
	void RotateTurret(FVector TargetLocation);
	void ShootProjectiles();
	void PerformLineTrace();
	void ShootAgainCooldown();
};
