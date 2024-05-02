// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnpoint.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "NiagaraComponent.h"

// Sets default values
AEnemySpawnpoint::AEnemySpawnpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
    RootComponent = CapsuleComponent;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComponent);

	PointDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("Point Direction"));
	PointDirection->SetupAttachment(CapsuleComponent);

	SpawnEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Spawn Effect"));
	SpawnEffect->SetupAttachment(CapsuleComponent);

	EnemyToSpawnClass = nullptr;
}

// Called when the game starts or when spawned
void AEnemySpawnpoint::BeginPlay()
{
	Super::BeginPlay();

	SpawnEffect->OnSystemFinished;
}

// Called every frame
void AEnemySpawnpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawnpoint::OnNiagaraSystemFinished()
{
	
}




