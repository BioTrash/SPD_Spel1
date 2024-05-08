// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyShooterAIController.generated.h"

/**
 * 
 */
class UNiagaraComponent;

UCLASS()
class SPD_SPEL1_API AEnemyShooterAIController : public AAIController
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(BlueprintReadOnly)
	FVector EnemyLocation;
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnEnemyShoot();
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UNiagaraSystem* ShootingEffect;
	
	
protected:

	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;


private:
	
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;
	
	void DetectPlayer(const FVector& PlayerLocation);
	void OnPlayerLocationUpdated(const FVector& NewPlayerLocation);

	UNiagaraComponent* NiagaraSystemComponent;
	float LastShotTime = 0.0f;

	// Cooldown duration between shots
	float ShootCooldown = 2.5f;

	bool EffectIsPlaying = false;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	class UStaticMeshComponent* EnemyStaticMeshComponent;
};