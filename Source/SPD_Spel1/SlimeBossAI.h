// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SlimeBossAI.generated.h"

UCLASS()
class SPD_SPEL1_API ASlimeBossAI : public APawn
{
	GENERATED_BODY()

public:

	ASlimeBossAI();

protected:

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void KillEnemy();
	void Shoot();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy")
	void OnEnemyDeath();

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 40.f;
	
	UPROPERTY(VisibleAnywhere)
	float Health;

private:

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_GameTraceChannel1;

};
