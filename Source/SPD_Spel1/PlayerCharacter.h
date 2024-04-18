// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.generated.h"


UCLASS()
class SPD_SPEL1_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//the health the player is initialized with (Rebecka)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 100;
	//health for player (Rebecka)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Health;

	UFUNCTION(BlueprintCallable)
	void Dash();
	UFUNCTION(BlueprintCallable)
	void DashUp();
	UFUNCTION(BlueprintCallable)
	void StopDash();

public:
	//declaration of function to connect WBP to the actual health of the player (Rebecka)
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	//function to connect WBP_DashCooldown and get the cooldown percentage for the dash
	UFUNCTION(BlueprintPure, Category= "Cooldown")
	float GetDashCooldownPercentage() const;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Taking damage method (Rebecka)
	UFUNCTION(BlueprintPure)
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure)
	AWeapon* GetTriggerWeapon() const;

private:
	void Slide();
	void StopSlide();
	void PerformDash();

	// Controls forward and backward movement (Rufus)
	void FrontBackMove(float AxisValue);
	// Controls right and left movement (Rufus)
	void RightLeftMove(float AxisValue);

	// Controls weapon swapping (Rufus)
	void SwapWeapon();
	// Controls weapon being fired, triggers 'PullTriger()' in 'Weapon.h'
	void Shoot();

	// Function and bool is required in order to determine when the player stops shooting continuously (Rufus)
	void CancelShoot();
	bool SprayShooting = true;

	// Triggers reloading manually in Weapon.cpp of 'TriggerWeapon'
	void ReloadWeapon();
	
	// Required for 'Shoot()'
	UPROPERTY()
	AWeapon* TriggerWeapon;
	
	//AActor should preferably be our own 'WeaponClass' instead (Rufus)
	//Due to how TSubclass functions AActors cannot be pointers, this is solved in .cpp file vie use of references
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weaponry", meta=(AllowPrivateAccess="True"))
	TArray<TSubclassOf<class AWeapon>> InitialWeaponArray;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weaponry", meta=(AllowPrivateAccess="True"))
	TArray<class AWeapon*> CurrentWeaponArray;

	//speed for the dash (can be changed in blueprints for the player) (Rebecka)
	UPROPERTY(EditAnywhere, Category="Dash")
	float DashForce = 1000.0f;
	//how long the dash lasts (can be changed in blueprints for the player) (Rebecka)
	UPROPERTY(EditAnywhere, Category="Dash")
	float DashDuration = 0.4f;
	//cooldown for the dash (can be changed in blueprints for the player) (Rebecka)
	UPROPERTY(EditAnywhere, Category="Dash")
	float DashCooldown = 2.0f;
	//keeping track of when the dash happened last (Rebecka)
	float LastDashTime = 0.0f;
	//useful for if-statement to keep track if the player is dashing or not (Rebecka)
	bool bIsDashing = false;
	//how much it slows the character down when dashing in the air
	float AirDashMultiplier = 0.7f;
	//handles the timer for the dash (Rebecka)
	FTimerHandle DashTimerHandle;
	float DashDelay = 0.01f;
	bool bHasDashed = false;
	
	UPROPERTY(EditAnywhere)
	UCameraComponent* FPSCamera;
	//how much the characters capsule gets rid of (Rebecka)
	float NewHalfHeight = 60.f;
	//keeping track of when the slide happened last (Rebecka)
	float LastSlideTime = 0.0f;
	//useful for if-statement to keep track if the player is sliding or not (Rebecka)
	bool bIsSliding = false;
	//how much the camera should move in Z when sliding (Rebecka)
	float SlideCameraOffset = 150.f;
	//how fast i will be moving when sliding (Rebecka)
	UPROPERTY(EditAnywhere, Category="Slide")
	float SlideSpeed = 2000.f;
	//how long the slide lasts (Rebecka)
	UPROPERTY(EditAnywhere, Category = "Slide")
	float SlideDuration = 0.5f;
	//how long you have to wait in order to slide again (Rebecka)
	UPROPERTY(EditAnywhere, Category="Slide")
	float SlideCooldown = 0.0f;

	UPROPERTY()
	FTimerHandle SliderTimerHandle;
	
	
};