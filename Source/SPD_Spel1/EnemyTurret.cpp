// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTurret.h"
#include "PlayerCharacter.h"
#include "MathUtil.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemyTurret::AEnemyTurret()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh ->SetupAttachment(BaseMesh);

	ProjectileSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Projectile"));
	ProjectileSpawn -> SetupAttachment(TurretMesh);

}

// Called when the game starts or when spawned
void AEnemyTurret::BeginPlay()
{
	Super::BeginPlay();
	APlayerCharacter* FoundPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	if (FoundPlayer)
	{
		Player = FoundPlayer;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player character not found!"));
	}
}

// Called every frame
void AEnemyTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Player)
	{
		float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("Distance to player: %f"), Distance);
	
	if(Distance <= FireRange)
	{
		RotateTurret(Player->GetActorLocation());
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is out of firing range")); // Debugging out of range
	}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player reference is not valid")); // Debugging player reference
	}
	}


// Called to bind functionality to input
void AEnemyTurret::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void AEnemyTurret::RotateTurret(FVector TargetLocation)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(TurretMesh->GetComponentLocation(), TargetLocation);
	LookAtRotation.Pitch = 0;
	LookAtRotation.Roll = 0;
	TurretMesh->SetWorldRotation(LookAtRotation);
    
	UE_LOG(LogTemp, Warning, TEXT("Turret rotation updated"));
	
}

