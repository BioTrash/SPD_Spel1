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
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void UpdatePlayerLocation();
	void LaunchLogic(float DeltaSeconds);
	void AnticipationLaunch(float DeltaSeconds, const FVector& EnemyLocation, const FVector& PlayerLocation, float DistanceToPlayer);
	void LaunchTowardsPlayer();
	void ExplodeAfterLaunch();
	
	UPROPERTY(EditAnywhere)
	UBehaviorTree* AIBehavior;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionDamage = 10.f;

	UPROPERTY(EditAnywhere, Category="Enemy")
	class UNiagaraSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, Category = "AI")
	float LaunchDistanceThreshold;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AnticipationDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MinLaunchDistance = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageRadius = 300.0f;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_GameTraceChannel1;
	
	bool bHasLaunched;
	bool bHasBeenDamagedByExplosion;

	FTimerHandle ExplodeTimerHandle;
};
