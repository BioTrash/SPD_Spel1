// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class SPD_SPEL1_API AWeaponBase : public AActor
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;

	template<typename T, typename U>
	void PullTrigger(bool bRapidFire, T Func, U* Object)
	{
		if (bRapidFire)
		{
			GetWorld()->GetTimerManager().SetTimer(RapidFireTimer, Object, Func, 0.1f, true, 0.0f);
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(RapidFireTimer);
		}
	}

	virtual void InitiateTimer(bool bButtonHeld) {} 
	virtual void Shoot() {}
	virtual void Reload() {}
	virtual void Spread() {}
	virtual void Recoil() {}
protected:
	AWeaponBase();
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	FTimerHandle RapidFireTimer;

	/* UE5 stubbornly refuses to delete operator and constructor complaining that they are already declared or defined somewhere, presumably in AActor.
	Technically not a problem as long as I trust people to understand the purpose and use case of an interface (Rufus) */
	//AWeaponBase(const AWeaponBase&) = delete;
	//const AWeaponBase& operator = (const AWeaponBase&) = delete;

};


