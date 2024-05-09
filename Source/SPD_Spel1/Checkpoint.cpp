// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Point Direction"));
	ArrowComponent->SetupAttachment(Root);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	BoxCollision->SetupAttachment(Root);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnBoxBeginOverlap);

	CheckpointText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Checkpoint"));
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	// Hämtar spelaren.
	if (APlayerCharacter* FoundPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Player = FoundPlayer;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player character not found!"));
	}
	
	// Hämtar gamemode.
	if (AGameModeBase* FoundGameMode = Cast<AGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode = FoundGameMode;
		//UE_LOG(LogTemp, Warning, TEXT("Gamemode Name: %s"), *GameMode->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Gamemode not found!"));
	}
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Körs när boxcollider overlappar.
void ACheckpoint::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	// Kollar om collidern overlappar med spelaren.
	if(OtherActor == Player)
	{
		//UE_LOG(LogTemp, Warning, TEXT("PLAYER OVERLAP"));
		
	}
}

