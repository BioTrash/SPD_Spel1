// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeProjectile.h"

#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASlimeProjectile::ASlimeProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));

	// Default values, can simply be changed in BlueprintEditor, in ProjectileMovementComponent. Will Default back to this on every build(?), may need to be deleted. (Rufus)
	ProjectileMovementComponent->MaxSpeed = 1300;
	ProjectileMovementComponent->InitialSpeed = 1300;

	bIsInPool = false;

}

// Called when the game starts or when spawned
void ASlimeProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true); //initially hide the projectile from start (Rebecka)
}

void ASlimeProjectile::InitiateProjectile()
{
	SetActorHiddenInGame(false); //show the projectile when initiated (Rebecka)
	bIsInPool = false;
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ASlimeProjectile::OnHit); // Bind hit event
}

void ASlimeProjectile::ReturnToPool()
{
	SetActorHiddenInGame(true); //hide the projectile when it's returned to the pool (Rebecka)
	bIsInPool = true;
	ProjectileMesh->OnComponentHit.RemoveDynamic(this, &ASlimeProjectile::OnHit); // Unbind hit event
	
}

// Called every frame
void ASlimeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handles hits, no clue what happens under the surface here. May need to be remade in to our own function (Rufus)
	//ProjectileMesh->OnComponentHit.AddDynamic(this, &ASlimeProjectile::OnHit);
}

void ASlimeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		// Apply damage to the other actor if it's not the projectile itself
		FPointDamageEvent DamageEvent(Damage, Hit, NormalImpulse, nullptr);
		OtherActor->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);
	}
	ReturnToPool();
}
