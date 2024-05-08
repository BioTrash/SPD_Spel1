// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SlimeBossAIController.generated.h"

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
	void FireCooldown();
	
private:

	UPROPERTY(EditAnywhere, Category="Turret")
	class APlayerCharacter* Player;

	UPROPERTY(EditAnywhere, Blueprintable, Category="Combat")
	float RotationSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category="Turret")
	float FiringRange = 1000.f;

	UPROPERTY(EditAnywhere)
	float NextShootTime = 0.f;

	UPROPERTY(EditAnywhere)
	float ShootCooldown = 4.f;

	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;
	
};
