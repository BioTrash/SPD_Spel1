// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RushEnemyAI.generated.h"

struct FDamageEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDeathDelegate);

UCLASS()
class SPD_SPEL1_API ARushEnemyAI : public ACharacter
{
	GENERATED_BODY()

public:
	ARushEnemyAI();
	
	void Explode(float Damage, bool bCollisionTriggered);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnEnemyDeath();
	
	UFUNCTION(BlueprintCallable, Category="Animations")
	bool GetIsLaunchingAnimation();

	bool IsLaunchingAnimation;
	bool bHasExploded;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
private:
	void PerformLineTrace();

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 20.f;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(VisibleAnywhere)
	float MaxTraceDistance = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageRadius = 300.0f;
	
	UPROPERTY(EditAnywhere, Category="Enemy")
	class UNiagaraSystem* ExplosionEffect;
	
	bool bCanAttack = true;
	FTimerHandle ExplodeCooldown;
	
};
