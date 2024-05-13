// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SlimeProjectile.generated.h"

UCLASS()
class SPD_SPEL1_API ASlimeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlimeProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Explode();
	void SetDamage(float NewDamage) { Damage = NewDamage; }
	
	UProjectileMovementComponent* GetProjectileMovementComponent();
	void SetProjectileMovementComponent(UProjectileMovementComponent *ProjMove);


private:
	UPROPERTY()
	class APlayerCharacter* Player;

	UPROPERTY(EditAnywhere)
	float SlimeDamageRadius;
	
	UPROPERTY(EditAnywhere)
	float Damage;
	UPROPERTY(EditDefaultsOnly, Category="Weaponry")
	UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(EditAnywhere, Category="Weaponry")
	class UNiagaraSystem* ExplosionEffect;

	// Sound like a custom component but is a part of Unreal, settings for it are found under any Projectile BlueprintEditor (Rufus)
	UPROPERTY(VisibleAnywhere, Category="Weaponry")
	UProjectileMovementComponent* ProjectileMovementComponent;

	// Is a copy of an existing Unreal function. Ngl, this one confuses me. It is used in Begin() to detect hits (Rufus)
	UFUNCTION(BlueprintPure)
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
