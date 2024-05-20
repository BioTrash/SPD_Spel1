#pragma once

#include "CoreMinimal.h"
#include "ProjectileWeapon.h"
#include "Weapon.h"
#include "GameFramework/Character.h"
#include "ShooterEnemy.generated.h"

UCLASS()
class SPD_SPEL1_API AShooterEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	
	AShooterEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool isAlive;
	
	void KillEnemy();

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	void SetRagdollPhysics();

	void SetHitInformation(FName BoneName, FVector Direction);

	UPROPERTY(Blueprintable, EditAnywhere)
	bool isShooting;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AProjectileWeapon> BP_EnemyWeaponClass;

	UPROPERTY(EditAnywhere)
	AProjectileWeapon* TriggerWeapon;

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnEnemyDeath();
	
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	UStaticMeshComponent* GetStaticMeshComponent() const;
	
	UFUNCTION(BlueprintCallable, Category="Attacking")
	bool getIsShooting();

private:

	FName HitBoneName;
	FVector HitDirection;

	float DeathTime;
	float DespawnCooldown = 3.0;

};