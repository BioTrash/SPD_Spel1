// Fill out your copyright notice in the Description page of Project Settings.

#include "SlimeProjectile.h"

#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

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

}

// Called when the game starts or when spawned
void ASlimeProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ASlimeProjectile::OnHit);
	

}

// Called every frame
void ASlimeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handles hits, no clue what happens under the surface here. May need to be remade in to our own function (Rufus)
	//ProjectileMesh->OnComponentHit.AddDynamic(this, &ASlimeProjectile::OnHit);

	if(!Player && Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
	
}

void ASlimeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		// Apply damage to the other actor if it's not the projectile itself
		//FPointDamageEvent DamageEvent(Damage, Hit, NormalImpulse, nullptr);
		//OtherActor->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);
	}
	//Destroy();
	this->Explode();
	this->SetActorHiddenInGame(true);
	this->SetActorEnableCollision(false);
	this->SetActorTickEnabled(false);
	
	ProjectileMovementComponent->SetActive(false);
}

void ASlimeProjectile::Explode()
{
	TArray<AActor*> OverlappingCharacters;
	FVector ExplosionLocation = GetActorLocation();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), OverlappingCharacters);

	for(AActor* Actor : OverlappingCharacters)
	{
		if(Cast<ACharacter>(Actor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy caught in explosion"));
			float DistanceToActor = FVector::Distance(ExplosionLocation, Actor->GetActorLocation());
			float DamageMultiplier = FMath::Clamp(1.0f - (DistanceToActor / SlimeDamageRadius), 0.0f, 1.0f); 
			float ActualDamage = Damage * DamageMultiplier;

			if(!Cast<APlayerCharacter>(Actor))
			{
				Actor->TakeDamage(ActualDamage, FDamageEvent(), GetInstigatorController(), this);	
			}
		}
	}

	if(ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, ExplosionLocation, FRotator::ZeroRotator, FVector::OneVector, true, true);
	}
}


UProjectileMovementComponent* ASlimeProjectile::GetProjectileMovementComponent()
{
	return ProjectileMovementComponent;
}

void ASlimeProjectile::SetProjectileMovementComponent(UProjectileMovementComponent *ProjMove)
{
	ProjectileMovementComponent = ProjMove;
}




