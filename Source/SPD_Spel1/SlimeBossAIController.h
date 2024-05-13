// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SlimeBossAI.h"
#include "Kismet/GameplayStatics.h"
#include "SlimeBossAIController.generated.h"

class UNiagaraComponent;
class AEnemySpawnpoint;

/**
 * 
 */
UCLASS()
class SPD_SPEL1_API ASlimeBossAIController : public AAIController
{
	GENERATED_BODY()
public:

	ASlimeBossAIController();
	virtual void Tick(float DeltaSeconds) override;
	void BeginPlay();
	void RotateHead(FVector TargetLocation);
	void Shoot();
	void SetPlayer();
	void UpdateBossPhase();
	void BossPhaseOne();
	void BossPhaseTwo();
	void BossPhaseThree();
	void SlamAttack();
	void SpawnEnemies();
	void EndSlamAttack();
	void ResetSlamAttack();
	
protected:
	
	FVector OriginalLocation;
	
private:

	UPROPERTY()
	UStaticMeshComponent* PawnMesh;

	UPROPERTY()
	UStaticMeshComponent* SlamMesh;

	UPROPERTY()
	FVector StartScale;
	
	UPROPERTY()
	FVector EndScale;

	UPROPERTY()
	float Alpha = 0.0f;

	UPROPERTY()
	float InterpolationSpeed = 0.5f;

	UPROPERTY()
	float SlamDamage;
	
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
	float LastSlamTime;

	UPROPERTY(EditAnywhere)
	float LastSpawnTime;

	UPROPERTY(EditAnywhere)
	float ShootCooldown = 1.0f;

	UPROPERTY(EditAnywhere)
	float SlamCooldown = 6.0f;
	
	FTimerHandle SlamCooldownTimerHandle;
	
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;
	
	UPROPERTY(EditAnywhere)
	TArray<AEnemySpawnpoint*> SpawnPointArray;

	UPROPERTY()
	ASlimeBossAI* Boss;
	
	bool bIsSlamming;
	bool bActivatePhaseTwo = true;
	bool bShouldSpawnEnemies = false;
	
	UPROPERTY()
	ASlimeBossAI* BossHealth;

	UPROPERTY()
	USceneComponent* ProjectileSpawn;
	
	float ProjectileDamage = 30;
	FTimerHandle SlamAttackTimerHandle;
	float DamageRadius = 1400.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;

	// Effect som spelas när bossen gör slam attack.
	UPROPERTY();
	UNiagaraComponent* SlamEffect;
	
	UPROPERTY()
	UNiagaraComponent* ShootEffect;

	UFUNCTION()
	void OnNiagaraSystemFinished(UNiagaraComponent* NiagaraComponent);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
