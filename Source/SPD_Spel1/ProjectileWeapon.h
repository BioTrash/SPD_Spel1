#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "GameFramework/Actor.h"
#include "ProjectileWeapon.generated.h"

class AGenericObjectPool;
class AProjectile;

UCLASS()
class SPD_SPEL1_API AProjectileWeapon : public AWeaponBase
{
    GENERATED_BODY()
    
public:    
    AProjectileWeapon();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Shoot();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void ShootEnemyProjectile();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void InitiateTimer(bool bButtonPressed, bool bAlternative);

private:
    UPROPERTY(EditAnywhere, Category = "Pool")
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Pool")
    AGenericObjectPool* ObjectPool;

};
