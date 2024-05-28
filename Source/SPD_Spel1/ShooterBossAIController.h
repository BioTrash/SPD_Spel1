#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ShooterEnemy.h"
#include "ProjectileWeapon.h"
#include "NiagaraComponent.h"
#include "ShooterBoss.h"
#include "ShooterBossAIController.generated.h"

/**
 * 
 */
class UNiagaraComponent;

UCLASS()
class SPD_SPEL1_API AShooterBossAIController : public AAIController
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

	void Shoot();

	//Attacks
	void SpamAttack();
	void JumpBack();
	int SpamAmount = 3.0f;
	float OriginalWalkSpeed;
	float NewWalkSpeed;
	
	UPROPERTY()
	AShooterBoss* Enemy;
	
	UPROPERTY()
	APawn* PlayerPawn;

	UPROPERTY()
	AProjectileWeapon* EnemyWeapon;
	
	FVector DirectionToPlayer;
	FRotator WeaponRotation;

	bool InPositionSpam = false;

	UNiagaraComponent* NiagaraSystemComponent;

	//Timers
	float LastShotTime = 0.0f;
	float LastSpamShotTime = 0.0f;

	void InitiateEnemy();
	void InitiatePlayer();
	
	// Cooldown duration between shots
	float ShootCooldown = 2.5f;
	float SpamShotCooldown = 0.3;

	bool EffectIsPlaying = false;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	class UStaticMeshComponent* EnemyStaticMeshComponent;
};