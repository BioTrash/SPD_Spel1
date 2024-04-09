// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Taking damage method (Rebecka)
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	//function for dash (Rebecka)
	void Dash();

	//function for stopdashing (Rebecka)
	void StopDash();
	
	// Controls forward and backward movement (Rufus)
	void FrontBackMove(float AxisValue);
	
	// Controls right and left movement (Rufus)
	void RightLeftMove(float AxisValue);

	// Controls weapon swapping (Rufus)
	void SwapWeapon();

	// Controls weapon being fired, triggers 'PullTriger()' in 'Weapon.h'
	void Shoot();
	
	// Required for 'Shoot()'
	UPROPERTY()
	class AWeapon* TriggerWeapon;
	
	//AActor should preferably be our own 'WeaponClass' instead (Rufus)
	//Due to how TSubclass functions AActors cannot be pointers, this is solved in .cpp file vie use of references
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weaponry", meta=(AllowPrivateAccess="True"))
	TArray<TSubclassOf<class AWeapon>> InitialWeaponArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weaponry", meta=(AllowPrivateAccess="True"))
	TArray<class AWeapon*> CurrentWeaponArray;

	//the health the player is initialized with (Rebecka)
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;

	//sets default values for dash (Rebecka)
	UPROPERTY(EditAnywhere, Category="Dash")
	float DashSpeed = 4000.0f;

	UPROPERTY(EditAnywhere, Category="Dash")
	float DashDuration = 0.4f;

	UPROPERTY(EditAnywhere, Category="Dash")
	float DashCooldown = 5.0f;

	float LastDashTime = 0.0f;
	bool bIsDashing = false;
	FTimerHandle DashTimerHandle;

};
