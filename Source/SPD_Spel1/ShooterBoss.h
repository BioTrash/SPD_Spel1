#pragma once

#include "CoreMinimal.h"
#include "ProjectileWeapon.h"
#include "Weapon.h"
#include "GameFramework/Character.h"
#include "ShooterBoss.generated.h"

UCLASS()
class SPD_SPEL1_API AShooterBoss : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterBoss();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool isAlive;
	
	void KillEnemy();

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	void SetRagdollPhysics();

	void SetHitInformation(FName BoneName, FVector Direction);

	UPROPERTY(Blueprintable, EditAnywhere)
	bool isShooting;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AProjectileWeapon> BP_EnemyWeaponClass;

	UPROPERTY(EditAnywhere)
	AProjectileWeapon* TriggerWeapon;

	UPROPERTY(EditAnywhere)
	AProjectileWeapon* WeaponInstance;

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnEnemyDeath();
	
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	UStaticMeshComponent* GetStaticMeshComponent() const;
	
	UFUNCTION(BlueprintCallable, Category="Attacking")
	bool getIsShooting();

	UFUNCTION(BlueprintCallable)
	void ResetRagdollPhysics();
	
	UFUNCTION(BlueprintCallable)
	void ResetHealth();

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetHealthPercentage() const;

	UFUNCTION(BlueprintCallable)
	AProjectileWeapon* GetTriggerWeapon() const;

private:

	FName HitBoneName;
	FVector HitDirection;

	float DeathTime;
	float DespawnCooldown = 2;


};