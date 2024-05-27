// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMesh->SetVisibility(true); // Ensure visibility is set to true
	ProjectileMesh->SetHiddenInGame(false); // Ensure it is not hidden in game
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	UE_LOG(LogTemp, Log, TEXT("Hej jag har spawnat!"));

	if (ProjectileMovementComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("Jag tycker MovementComponent funkar"));
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			UE_LOG(LogTemp, Log, TEXT("Jag tycker Playerpawn funkar funkar"));
			FVector Direction = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
		}
	}
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (OtherActor && PlayerPawn && OtherActor == PlayerPawn)
	{
		FPointDamageEvent DamageEvent(Damage, Hit, NormalImpulse, nullptr);
		PlayerPawn->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);
	}

	DeactivateProjectile();
}

void AProjectile::DeactivateProjectile()
{
	this->SetActorHiddenInGame(true);
	this->SetActorEnableCollision(false);
	this->SetActorTickEnabled(false);
	ProjectileMovementComponent->SetActive(false);

	if (OwningPool)
	{
		OwningPool->ReturnPooledObject(this);
	}
}

UProjectileMovementComponent* AProjectile::GetProjectileMovementComponent() const
{
	return ProjectileMovementComponent;
}

void AProjectile::SetProjectileMovementComponent(UProjectileMovementComponent *ProjMove)
{
	ProjectileMovementComponent = ProjMove;
}





