// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class SPD_SPEL1_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Weaponry")
	UStaticMeshComponent* ProjectileMesh;

	// Sound like a custom component but is a part of Unreal, settings for it are found under any Projectile BlueprintEditor (Rufus)
	UPROPERTY(VisibleAnywhere, Category="Weaponry")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// Is a copy of an existing Unreal function. Ngl, this one confuses me. It is used in Begin() to detect hits (Rufus)
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
