// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Spel1PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SPD_SPEL1_API ASpel1PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void AdvanceTime();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> AmmoClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> CrosshairClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> TimerClass;

	UPROPERTY()
	UUserWidget *Ammo;
	UPROPERTY()
	UUserWidget *Crosshair;
	UPROPERTY()
	UUserWidget *Timer;
	
	UPROPERTY()
	FTimerHandle SinceStartTimer;
	UPROPERTY()
	FTimerHandle SinceDeathTimer;
	
	UPROPERTY()
	int TimePassed = 0;
};
