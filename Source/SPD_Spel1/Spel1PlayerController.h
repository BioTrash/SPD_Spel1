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
	UFUNCTION()
	void SlimeChargeProgress();

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

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> SlimeChargeClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> FadeInClass;

	UPROPERTY()
	UUserWidget *Ammo;
	UPROPERTY()
	UUserWidget *Crosshair;
	UPROPERTY()
	UUserWidget *Timer;
	UPROPERTY()
	UUserWidget *SlimeCharge;
	UPROPERTY()
	UUserWidget *FadeIn;
	
	UPROPERTY()
	FTimerHandle SinceStartTimer;
	UPROPERTY()
	FTimerHandle SinceDeathTimer;
	UPROPERTY()
	FTimerHandle ChargeSlimeTimer;
	
	UPROPERTY()
	int TimePassed = 0;
	UPROPERTY()
	int ChargeTimePassed = 0;

	bool TimerActivate = true;
};
