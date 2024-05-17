// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RushEnemyAI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDeathDelegate);

UCLASS()
class SPD_SPEL1_API ARushEnemyAI : public ACharacter
{
	GENERATED_BODY()

public:
	ARushEnemyAI();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void Explode(float Damage, bool bCollisionTriggered);

	bool bHasExploded;
	
	UPROPERTY(BlueprintAssignable, Category = "Enemy")
	FOnEnemyDeathDelegate OnEnemyDeathDelegate;
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 20.f;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnEnemyDeath();
	
	float MaxTraceDistance = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageRadius = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Jump")
	float JumpForce = 1000.0f;

	bool IsLaunchingAnimation;
	
	UFUNCTION(BlueprintCallable, Category="Animations")
	bool GetIsLaunchingAnimation();
	
private:
	bool bCanAttack = true;
	FTimerHandle ExplodeCooldown;
	
	void PerformLineTrace();
	
	UPROPERTY(EditAnywhere, Category="Enemy")
	class UNiagaraSystem* ExplosionEffect;

};
