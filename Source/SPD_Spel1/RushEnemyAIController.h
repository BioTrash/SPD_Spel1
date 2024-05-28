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
	UPROPERTY(EditAnywhere)
	UBehaviorTree* AIBehavior;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float AnticipationDelay = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float LaunchSpeed = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float JumpHeight = 600.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionDamage = 20.f;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float LaunchDistanceThreshold;
	
	UPROPERTY(EditAnywhere, Category="Enemy")
	class UNiagaraSystem* ExplosionEffect;

	bool bHasLaunched;
	FTimerHandle ExplodeTimerHandle;

	void UpdatePlayerLocation();
	void LaunchLogic(float DeltaSeconds);
	void AnticipationLaunch(float DeltaSeconds, const FVector& EnemyLocation, const FVector& PlayerLocation, float DistanceToPlayer);
	void LaunchTowardsPlayer();
	void ExplodeAfterLaunch();
};
