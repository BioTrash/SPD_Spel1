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
	// Sets default values for this character's properties
	AShooterEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool isAlive;
	bool IsTakingDamage;
	
	void DestroyActor();
	
	void KillEnemy();

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	void SetRagdollPhysics();

	void SetHitInformation(FName BoneName, FVector Direction, bool SlimeShot);

	UPROPERTY(Blueprintable, EditAnywhere)
	bool isShooting;
	
	UPROPERTY(Blueprintable, EditAnywhere)
	bool IsCharging = false;

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

	UFUNCTION(BlueprintCallable, Category="Attacking")
	bool GetIsCharging();

	UFUNCTION(BlueprintCallable, Category="Damage")
	bool GetIsTakingDamage();


private:

	FName HitBoneName;
	FVector HitDirection;
	bool HitBySlime;

	float DeathTime;
	float DespawnCooldown = 3.0;

};