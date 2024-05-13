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
	void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	void LaunchTowardsPlayer();
	void ExplodeAfterLaunch();
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageRadius = 300.0f;
	bool bHasBeenDamagedByExplosion; 
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float LaunchDistanceThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AnticipationDelay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MinLaunchDistance = 300.0f;

private:
	bool bHasLaunched;
	FTimerHandle ExplodeTimerHandle;
	
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionDamage = 10.f;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_GameTraceChannel1;

	UPROPERTY(EditAnywhere, Category="Enemy")
	class UNiagaraSystem* ExplosionEffect;
};
