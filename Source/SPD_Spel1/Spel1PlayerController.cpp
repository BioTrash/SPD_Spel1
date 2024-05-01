// Fill out your copyright notice in the Description page of Project Settings.


#include "Spel1PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
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

	GetWorld()->GetTimerManager().SetTimer(SinceStartTimer, this, &ASpel1PlayerController::AdvanceTime, 0.001f, true, 0.0f);
}

void ASpel1PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
