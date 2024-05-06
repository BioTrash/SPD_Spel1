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

	ARushEnemyAIController();
	virtual void Tick(float DeltaSeconds) override;
	void BeginPlay();
	void LaunchTowardsPlayer();
	bool EnemyGrounded();
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageRadius = 300.0f;

	bool bHasBeenDamagedByExplosion; 
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float JumpVelocity = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float LaunchDistanceThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AnticipationDelay = 2.0f;


private:
	bool bHasLaunched;
	
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionDamage;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_GameTraceChannel1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionRadius;
};
