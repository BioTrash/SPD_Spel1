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
	void SlimeCharge();
	void InitiateReload();
	bool bRightButtonPressed;

	// This needs to be in the header because template needs to be available at initialization, i.e. before compilation. (Rufus)
	template<typename T, typename U>
	void PullTrigger(bool bButtonPressed, bool bAlternative, T Func, U* Object)
	{
		if (bButtonPressed)
		{
			// Start the timer for delayed fire
			if (bDelayed && !bAlternative)
			{
				GetWorld()->GetTimerManager().SetTimer(RapidFireTimer, Object, Func, FireRate, false, 0.0f);
				bDelayed = false;
				GetWorld()->GetTimerManager().SetTimer(FireDelayTimer, this, &AWeaponBase::DelaySwitch, FireDelay/2, false, FireDelay/2);
			}
		}
		// Start the alternative fire timer
		if (bAlternative && !bRightButtonPressed)
		{
			StartAlternativeFireTimer(Func, Object);
			bRightButtonPressed = true;

		}
		else
		{
			// Stop the alternative fire timer when the button is released
			if (bAlternative && bRightButtonPressed)
			{
				StopAlternativeFireTimer();
				bRightButtonPressed = false;
			}
			bButtonReleased = true;
		}
	}
	
	template<typename T, typename U>
	void StartAlternativeFireTimer(T Func, U* Object)
	{
		if (!bSlimeCharged)
		{
			GetWorld()->GetTimerManager().SetTimer(AlternativeFireTimer, Object, Func, MaxChargeTime, false);
			bAlternativeFireTimerActive = true;
			bRightButtonPressed = true;
		}
	}

	// Helper function to stop the alternative fire timer
	void StopAlternativeFireTimer()
	{
		// Clear the timer if the right mouse button is released
		GetWorld()->GetTimerManager().ClearTimer(AlternativeFireTimer);
		bAlternativeFireTimerActive = false;
		bButtonReleased = true;
	}
	
	void Reload()
	{
		if(!bReloading)
		{
			bReloading = true;
			bDelayed = false;
			GetWorld()->GetTimerManager().SetTimer(ReloadDelayTimer, this, &AWeaponBase::InitiateReload, ReloadDelay/2, false, ReloadDelay/2);
		}
	}
	
	virtual void InitiateTimer(bool bButtonHeld, bool bAlternative) {} 
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
	UFUNCTION(BlueprintPure)
	USceneComponent* GetMuzzlePoint() const;
	
	UFUNCTION(BlueprintCallable)
	void SetbUnlimitedAmmo(bool _bUnlimitedAmmo);
	UFUNCTION(BlueprintCallable)
	void SetTotalAmmo(int32 _TotalAmmo);
	UFUNCTION(BlueprintCallable)
	void SetCurrentClip(int32 _CurrentClip);
	UFUNCTION(BlueprintCallable)
	void SetClipSize(int32 _ClipSize);
	UFUNCTION(BlueprintCallable)
	void SetFireRate(int32 _FireRate);
	
	UFUNCTION(BlueprintImplementableEvent)
	void WhenShot();
	
	bool bButtonReleased = false;
	bool bSlimeCharged = false;
	bool bAlternativeFireTimerActive;
protected:
	AWeaponBase();
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="Weapon Beahviour") // Charge time for slime alternative fire
	float MaxChargeTime = 2.0f;
private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* MuzzlePoint;

	
	UPROPERTY()
	FTimerHandle RapidFireTimer;
	
	UPROPERTY()
	FTimerHandle FireDelayTimer;
	UPROPERTY()
	bool bDelayed = true;
	
	
	UPROPERTY()
	FTimerHandle ReloadDelayTimer;
	UPROPERTY()
	bool bReloading = false;

	UPROPERTY()
	FTimerHandle AlternativeFireTimer;

	

	
	UPROPERTY(EditAnywhere, Category="Weapon Behaviour")
	float FireRate = 0.1f;
	UPROPERTY(EditAnywhere, Category="Weapon Behaviour")
	float FireDelay = 0.0f;
	UPROPERTY(EditAnywhere, Category="Weapon Behaviour")
	float ReloadDelay = 0.0f;
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


