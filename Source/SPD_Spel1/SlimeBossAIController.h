// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SlimeBossAI.h"
#include "Kismet/GameplayStatics.h"
#include "SlimeBossAIController.generated.h"

/**
 * 
 */
UCLASS()
class SPD_SPEL1_API ASlimeBossAIController : public AAIController
{
	GENERATED_BODY()
public:
	
	virtual void Tick(float DeltaSeconds) override;
	void BeginPlay();
	void RotateHead(FVector TargetLocation);
	void Shoot();
	void FireCooldown();

	void SetPlayer();

private:

	UPROPERTY()
	UStaticMeshComponent* PawnMesh;
	
	UPROPERTY(EditAnywhere, Category="Turret")
	class APawn* Player;

	UPROPERTY(EditAnywhere, Blueprintable, Category="Combat")
	float RotationSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category="Turret")
	float FiringRange = 1000.f;

	UPROPERTY(EditAnywhere)
	float NextShootTime = 0.f;

	UPROPERTY(EditAnywhere)
	float LastShotTime;

	UPROPERTY(EditAnywhere)
	float ShootCooldown = 1.4f;

	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;

	UPROPERTY()
	ASlimeBossAI* Boss;

	UPROPERTY()
	USceneComponent* ProjectileSpawn;
	
	float ProjectileDamage = 30;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;

	
};
