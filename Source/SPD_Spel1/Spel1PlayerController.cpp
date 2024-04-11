// Fill out your copyright notice in the Description page of Project Settings.


#include "Spel1PlayerController.h"
#include "Blueprint/UserWidget.h"

void ASpel1PlayerController::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget *Ammo = CreateWidget(this, AmmoClass);
	if(AmmoClass) Ammo->AddToViewport();

	UUserWidget *Crosshair = CreateWidget(this, CrosshairClass);
	if(CrosshairClass) Crosshair->AddToViewport();
}
