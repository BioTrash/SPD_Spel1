// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RushEnemyAI.generated.h"

UCLASS()
class SPD_SPEL1_API ARushEnemyAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARushEnemyAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void KillEnemy();
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnEnemyDeath();
	
	float MaxTraceDistance = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageRadius = 200.0f;
	
private:
	void PerformLineTrace();
	UFUNCTION()
	void DealDamageToPlayer(float Damage);
	void Explode();
	void EndExplodeCooldown();

	bool bCanAttack = true;
	FTimerHandle ExplodeCooldown;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_GameTraceChannel1;
};
