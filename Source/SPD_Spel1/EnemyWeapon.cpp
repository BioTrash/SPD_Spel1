// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWeapon.h"

#include <UIAutomationCore.h>

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	Projectile = this->Bullet;
	
}

void AEnemyWeapon::FireAtPlayer(float DamageDone)
{
	
}