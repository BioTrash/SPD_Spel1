#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ShooterEnemy.h"
#include "ProjectileWeapon.h"
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

	void Shoot();
	UPROPERTY()
	AShooterEnemy* Enemy;
	
	UPROPERTY()
	APawn* PlayerPawn;

	UPROPERTY()
	AProjectileWeapon* EnemyWeapon;
	
	FVector DirectionToPlayer;
	FRotator WeaponRotation;

	void UpdateRePositionLocation();
	
	void DetectPlayer(const FVector& PlayerLocation);
	void BeginChase(bool ChaseStatus);

	bool ShootTime = false;

	UFUNCTION()
	void OnPlayerLocationUpdated(const FVector& NewPlayerLocation);

	UFUNCTION()
	void OnChaseUpdated(const bool NewChaseStatus);

	UNiagaraComponent* NiagaraSystemComponent;
	float LastShotTime = 0.0f;

	void InitiateEnemy();
	void InitiatePlayer();
	
	float ShootCooldown = 2.5f;

	bool EffectIsPlaying = false;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	class UStaticMeshComponent* EnemyStaticMeshComponent;
};