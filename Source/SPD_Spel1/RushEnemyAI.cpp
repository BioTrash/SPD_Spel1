// Fill out your copyright notice in the Description page of Project Settings.


#include "RushEnemyAI.h"
#include "PlayerCharacter.h"
#include "PlayerCharacter.h"

// Sets default values
ARushEnemyAI::ARushEnemyAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARushEnemyAI::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARushEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARushEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARushEnemyAI::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("Enemy damaged the player"));
	
	if(PlayerCharacter)
	{
		
	}
}


