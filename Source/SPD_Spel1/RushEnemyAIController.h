// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RushEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SPD_SPEL1_API ARushEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	virtual void Tick(float DeltaSeconds) override;
	
	void Explode(float Damage);
	void LaunchTowardsPlayer();
	void KillEnemy();
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageRadius = 300.0f;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AcceptanceRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float JumpVelocity = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float LaunchDistanceThreshold = 500.f;

private:

	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionRadius;
};
