// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyShooterAIController.generated.h"

/**
 * 
 */
UCLASS()
class SPD_SPEL1_API AEnemyShooterAIController : public AAIController
{
	GENERATED_BODY()

	public:

	virtual void Tick(float DeltaSeconds) override;
	
protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;
	
};
