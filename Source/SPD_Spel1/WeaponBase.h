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

	void DelaySwitch();

	// This needs to be in the header because template needs to be available at initialization, i.e. before compilation. (Rufus)
	template<typename T, typename U>
	void PullTrigger(bool bRapidFire, T Func, U* Object)
	{
		if (bRapidFire)
		{
			if(bDelayed)
			{
				GetWorld()->GetTimerManager().SetTimer(RapidFireTimer, Object, Func, FireRate, true, 0.0f);
				bDelayed = false;
				GetWorld()->GetTimerManager().SetTimer(FireDelayTimer, this, &AWeaponBase::DelaySwitch, FireDelay/2, false, FireDelay/2);
			}
		}
		else
		{
			if(!bDelayed) return;
			GetWorld()->GetTimerManager().ClearTimer(RapidFireTimer);
		}
	}
	
	virtual void Reload();

	virtual void InitiateTimer(bool bButtonHeld) {} 
	virtual void Shoot() {}
	virtual void Spread() {}
	virtual void Recoil() {}
	

	UFUNCTION(BlueprintPure)
	bool GetbUnlimitedAmmo() const;
	UFUNCTION(BlueprintPure)
	int32 GetTotalAmmo() const;
	UFUNCTION(BlueprintPure)
	int32 GetCurrentClip() const;
	UFUNCTION(BlueprintPure)
	int32 GetClipSize() const;
	UFUNCTION(BlueprintPure)
	AController* GetOwnerController() const;
	UFUNCTION(BlueprintPure)
	APawn* GetOwnerCharacter() const;
	UFUNCTION(BlueprintPure)
	FVector GetLocation() const;
	UFUNCTION(BlueprintPure)
	FRotator GetRotation() const;
	UFUNCTION(BlueprintPure)
	FString GetAmmo() const;

	UFUNCTION(BlueprintCallable)
	void SetbUnlimitedAmmo(bool _bUnlimitedAmmo);
	UFUNCTION(BlueprintCallable)
	void SetTotalAmmo(int32 _TotalAmmo);
	UFUNCTION(BlueprintCallable)
	void SetCurrentClip(int32 _CurrentClip);
	UFUNCTION(BlueprintCallable)
	void SetClipSize(int32 _ClipSize);
	
	UFUNCTION(BlueprintImplementableEvent)
	void WhenShot();


	
protected:
	AWeaponBase();
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	FTimerHandle RapidFireTimer;
	FTimerHandle FireDelayTimer;
	UPROPERTY()
	bool bDelayed = true;
	
	UPROPERTY(EditAnywhere, Category="Fire Behaviour")
	float FireRate = 0.1f;
	UPROPERTY(EditAnywhere, Category="Fire Behaviour")
	float FireDelay = 0.0f;
	UPROPERTY(EditAnywhere, Category="Ammo")
	bool bUnlimitedAmmo = false;
	UPROPERTY(EditAnywhere, Category="Ammo")
	int32 TotalAmmo;
	UPROPERTY(EditAnywhere, Category="Ammo")
	int32 CurrentClip;
	UPROPERTY(EditAnywhere, Category="Ammo")
	int32 ClipSize;

	UPROPERTY()
	AController* OwnerController;
	UPROPERTY()
	APawn* OwnerCharacter;

	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotation;

	/* UE5 stubbornly refuses to delete operator and constructor complaining that they are already declared or defined somewhere, presumably in AActor.
	Technically not a problem as long as I trust people to understand the purpose and use case of an interface (Rufus) */
	//AWeaponBase(const AWeaponBase&) = delete;
	//const AWeaponBase& operator = (const AWeaponBase&) = delete;

};


