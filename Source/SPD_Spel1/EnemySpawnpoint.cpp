// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnpoint.h"
#include "Components/ArrowComponent.h"
#include "NiagaraComponent.h"
//#include "Engine/World.h"

// Sets default values
AEnemySpawnpoint::AEnemySpawnpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	BaseMesh->SetupAttachment(Root);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Point Direction"));
	ArrowComponent->SetupAttachment(Root);

	SpawnEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Spawn Effect"));
	SpawnEffect->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AEnemySpawnpoint::BeginPlay()
{
	Super::BeginPlay();
	// Hämtar location och rotation.
	Location = GetActorLocation();
	Rotation = GetActorRotation();
	World = GetWorld();
	
	// Kopplar metoden OnNiagaraSystemFinished till evented OnSystemFinished (Kör metoden när effekten är klar).
	SpawnEffect->OnSystemFinished.AddDynamic(this, &AEnemySpawnpoint::OnNiagaraSystemFinished);
}

// Called every frame
void AEnemySpawnpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Körs när effekten har spelat klart.
void AEnemySpawnpoint::OnNiagaraSystemFinished(UNiagaraComponent* NiagaraComponent)
{
	if(World && EnemyToSpawnClass)
	{
		// Sätter collision params till att alltid spawna.
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		// Spawnar fienden.
		World->SpawnActor<AActor>(EnemyToSpawnClass, Location, Rotation, SpawnParams);

	}
}








