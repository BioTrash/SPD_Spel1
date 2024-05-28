#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;
class AGenericObjectPool;

UCLASS()
class SPD_SPEL1_API AProjectile : public AActor
{
	GENERATED_BODY()
    
public:    
	AProjectile();

protected:
	virtual void BeginPlay() override;

public:    
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UProjectileMovementComponent* GetProjectileMovementComponent() const;

	void SetProjectileMovementComponent(UProjectileMovementComponent* ProjMove);

	void SetOwningPool(AGenericObjectPool* Pool);

private:
	UPROPERTY(EditAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage;

	AGenericObjectPool* OwningPool;

	void DeactivateProjectile();
};
