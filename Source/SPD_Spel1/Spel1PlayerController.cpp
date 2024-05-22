// Fill out your copyright notice in the Description page of Project Settings.


#include "Spel1PlayerController.h"

#include "AlternativeFireMode.h"
#include "NonProjectileWeapon.h"
#include "PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ASpel1PlayerController::BeginPlay()
{
	Super::BeginPlay();

	Ammo = CreateWidget(this, AmmoClass);
	if(AmmoClass) Ammo->AddToViewport();

	Crosshair = CreateWidget(this, CrosshairClass);
	if(CrosshairClass) Crosshair->AddToViewport();

	Timer = CreateWidget(this, TimerClass);
	if(TimerClass) Timer->AddToViewport();

	SlimeCharge = CreateWidget(this, SlimeChargeClass);
	if(SlimeChargeClass) SlimeCharge->AddToViewport();

	FadeIn = CreateWidget(this, FadeInClass);
	if(FadeInClass) FadeIn->AddToViewport();

	GetWorld()->GetTimerManager().SetTimer(SinceStartTimer, this, &ASpel1PlayerController::AdvanceTime, 0.001f, true, 0.0f);
}

void ASpel1PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(this->GetPawn());

	if (PlayerCharacter)
	{
		AWeaponBase* TriggerWeapon = PlayerCharacter->GetTriggerWeapon();

		if (TriggerWeapon)
		{
			ANonProjectileWeapon* NonProjectileWeapon = Cast<ANonProjectileWeapon>(TriggerWeapon);

			if (NonProjectileWeapon)
			{
				AAlternativeFireMode* AlternativeFireMode = Cast<AAlternativeFireMode>(NonProjectileWeapon);
				
				if (AlternativeFireMode)
				{
					if(AlternativeFireMode->bAlternativeFireTimerActive)
					{
						if(TimerActivate)
						{
							TimerActivate = false;
							GetWorld()->GetTimerManager().SetTimer(ChargeSlimeTimer, this, &ASpel1PlayerController::SlimeChargeProgress, 0.001f, true, 0.0f);
						}
					}
					else
					{
						TimerActivate = true;
						ChargeTimePassed = 0;
						SlimeChargeProgress();
						GetWorld()->GetTimerManager().ClearTimer(ChargeSlimeTimer);
					}
				}
			}
		}
	}
}

void ASpel1PlayerController::AdvanceTime()
{
	TimePassed++;

	int32 MilliSecond = TimePassed % 1000;
	int32 Second = (TimePassed / 1000) % 60;
	int32 Minute = (TimePassed / 60000) % 60;

	FString TimeElapsedString = FString::Printf(TEXT("%02d:%02d:%03d"), Minute, Second, MilliSecond);
    
	FText TimeElapsedText = FText::FromString(TimeElapsedString);
    
	UTextBlock* TotalTimeElapsed = Timer->WidgetTree->FindWidget<UTextBlock>(TEXT("TotalTimeElapsed"));
	TotalTimeElapsed->SetText(TimeElapsedText);
}

void ASpel1PlayerController::SlimeChargeProgress()
{
	ChargeTimePassed += 2;
	
	float SlimeChargePercentage = FMath::Clamp((float)ChargeTimePassed / 2000.0f, 0.0f, 1.0f);
	if (UProgressBar* SlimeChargeBar = SlimeCharge->WidgetTree->FindWidget<UProgressBar>(TEXT("SlimeChargeBar")))
	{
		SlimeChargeBar->SetPercent(SlimeChargePercentage);
	}
}

