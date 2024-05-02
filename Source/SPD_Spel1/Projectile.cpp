// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));

	// Default values, can simply be changed in BlueprintEditor, in ProjectileMovementComponent. Will Default back to this on every build(?), may need to be deleted. (Rufus)
	//ProjectileMovementComponent->MaxSpeed = 1300;
	//ProjectileMovementComponent->InitialSpeed = 1300;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (ProjectileMovementComponent)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			FVector Direction = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			UE_LOG(LogTemp, Error, TEXT("VELOCITY EQUALS : %s"), *FString(Direction.ToString()));
			ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
		}
	}
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handles hits, no clue what happens under the surface here. May need to be remade in to our own function (Rufus)
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		// Apply damage to the other actor if it's not the projectile itself
		FPointDamageEvent DamageEvent(Damage, Hit, NormalImpulse, nullptr);
		OtherActor->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);
	}
	Destroy();
}




