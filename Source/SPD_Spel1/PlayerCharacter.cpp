// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "WeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Tasks/Task.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//finding and assign the camera component
	TArray<UCameraComponent*> CameraComponents;
	GetComponents<UCameraComponent>(CameraComponents);
	if(CameraComponents.Num() > 0)
	{
		FPSCamera = CameraComponents[0];
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("No camera component found on PlayerCharacter"));
	}

	//Start with the MaxHealth when starting the level (Rebecka)
	Health = MaxHealth;

	// Checks whether player is suppose to have any weapons at the start or not (Rufus)
	if(!InitialWeaponArray.IsEmpty())
	{
		// Goes through the entire array of weapon blueprints (Rufus)
		for(TSubclassOf<AWeaponBase>& Weapon : InitialWeaponArray) 
		{
			// Spawns in each weapon blue print at zero world position (Rufus)
			AWeaponBase* WeaponInstance = GetWorld()->SpawnActor<AWeaponBase>(*Weapon, FVector::ZeroVector, FRotator::ZeroRotator);

			// Checks whether spawn was successful or not (Rufus)
			if(WeaponInstance)
			{
				// Any spawned weapons that wasn't at first '[0]' position in InitialWeaponArray is hidden (Rufus)
				if(Weapon != InitialWeaponArray[0])
				{
					// SetHidden(true) causes problems, SetActorHiddenInGame(true) is recommended by Unreal itself (Rufus)
					WeaponInstance->SetActorHiddenInGame(true);
				}

				// Socket is temporary although will probably still be named the same (Rufus)
				// Transform is messed up but ought to wait for the player mesh to be made (Rufus)
				WeaponInstance->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
				CurrentWeaponArray.Push(WeaponInstance);

				// Required by 'PullTriger' in 'Weapon.cpp' (Rufus)
				TriggerWeapon = WeaponInstance;
				TriggerWeapon->SetOwner(this);
				
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon array is empty"));
	}
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
// Definitions can be found at: 'Project Settings -> Engine -> Input' (Rufus)
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// TEXT field has to have the exact same name as the AxisMapping, functions have to references - '&' (Rufus)
	// 'this' refers to this, player, object but could theoretically be any object; possible mechanic for controlling several objects at once (Rufus)
	PlayerInputComponent->BindAxis(TEXT("FrontBackMove"), this, &APlayerCharacter::FrontBackMove);
	PlayerInputComponent->BindAxis(TEXT("RightLeftMove"), this, &APlayerCharacter::RightLeftMove);

	//Pitch is inverted, i.e. looking up is -1 and looking down is +1 (Rufus)
	PlayerInputComponent->BindAxis(TEXT("LookUpDown"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRightLeft"), this, &APawn::AddControllerYawInput);
	
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("SwapWeapon"), EInputEvent::IE_Pressed, this, &APlayerCharacter::SwapWeapon);

	//Binding for dash (Rebecka)
	PlayerInputComponent->BindAction(TEXT("Dash"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Dash);
	//Binding for lide (Rebecka)
	PlayerInputComponent->BindAction(TEXT("Slide"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Slide);
	
	
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Released, this, &APlayerCharacter::CancelShoot);

	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &APlayerCharacter::ReloadWeapon);
	
}

void APlayerCharacter::Shoot()
{
	SprayShooting = true;
	TriggerWeapon->InitiateTimer(SprayShooting);
}

void APlayerCharacter::CancelShoot()
{
	SprayShooting = false;
	TriggerWeapon->InitiateTimer(SprayShooting);
}

void APlayerCharacter::ReloadWeapon()
{
	TriggerWeapon->Reload();
}

AWeaponBase* APlayerCharacter::GetTriggerWeapon() const
{
	return TriggerWeapon;
}


// AxisValue is +1 if moving forward and -1 if moving backwards (Rufus)
void APlayerCharacter::FrontBackMove(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

// AxisValue is +1 if moving right and -1 if moving left (Rufus)
void APlayerCharacter::RightLeftMove(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void APlayerCharacter::SwapWeapon()
{
	if(!CurrentWeaponArray.IsEmpty())
	{
		AWeaponBase* lastElement = CurrentWeaponArray.Last(); // Save the last element
    
		// Shift elements to the right
		for (int i = CurrentWeaponArray.Num() - 1; i > 0; --i) {
			CurrentWeaponArray[0]->SetActorHiddenInGame(true);
			CurrentWeaponArray[i] = CurrentWeaponArray[i - 1];
			
		}

		// Move the last element to the beginning
		CurrentWeaponArray[0] = lastElement;
		CurrentWeaponArray[0]->SetActorHiddenInGame(false);
		TriggerWeapon = CurrentWeaponArray[0];
		
	}
}

//method for dashing (Rebecka)
void APlayerCharacter::Dash()
{
	UE_LOG(LogTemp, Warning, TEXT("Dash called"));
	if (!bIsDashing && (GetWorld()->GetTimeSeconds() - LastDashTime) > DashCooldown)
	{
		if (GetCharacterMovement())
		{
			//get the players velocity so we can dash in the direction the player is moving (Rebecka)
			FVector PlayerVelocity = GetCharacterMovement()->Velocity;

			//if the player is not moving (or velocity is close to 0) the forward vector insted (Rebecka)
			if(PlayerVelocity.SizeSquared() < SMALL_NUMBER)
			{
				PlayerVelocity = GetActorForwardVector();
			}

			//normalize the dash direction by mulitplying it to the dash speed (Rebecka)
			FVector DashDirection = PlayerVelocity.GetSafeNormal() * DashSpeed;

			//checks if the character is grounded
			bool bIsGrounded = GetCharacterMovement()->IsMovingOnGround();

			//if the character is grounded, the value will be 1, if its not grounded it will be the value of AirDashMulitplier
			float LaunchMultiplier = bIsGrounded ? 1.0f : AirDashMultiplier;

			//apply dash velocity to the character. Depending on if its grounded or not it will have different speeds (can tweak the speed)
			GetCharacterMovement()->Launch(DashDirection * LaunchMultiplier);
			
			bIsDashing = true;
			LastDashTime = GetWorld()->GetTimeSeconds();

			//sets a timer for how long the dash lasts
			GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacter::StopDash, DashDuration, false);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Character movement component not found"));
		}
	}
}

//method to stop dashing and sets the bool to false to help resetting dash efter duration (Rebecka)
void APlayerCharacter::StopDash()
{
	bIsDashing = false;
}

//method for slide movement for player (Rebecka)
void APlayerCharacter::Slide()
{
	//check if the method is being called
	UE_LOG(LogTemp, Warning, TEXT("Sliding called"));
	//checking if the character is currently grounded and not dashing (Rebecka)
	if(GetCharacterMovement()->IsMovingOnGround() && !bIsDashing)
		if(!bIsSliding && (GetWorld()->GetTimeSeconds() - LastSlideTime) > SlideCooldown) {
			{
				FVector PlayerVelocity = GetCharacterMovement()->Velocity;
				if(PlayerVelocity.SizeSquared() > FMath::Square(0.1f))
				{
					//reducing the characters capsule by half its height to simulate sliding (Rebecka)
					GetCapsuleComponent()->SetCapsuleHalfHeight(NewHalfHeight, true);

					//adjust the camera position (Rebecka)
					if(FPSCamera)
					{
						FVector NewCameraLocation = FPSCamera->GetComponentLocation();
						NewCameraLocation.Z -= SlideCameraOffset;
						FPSCamera->SetWorldLocation(NewCameraLocation);
					}
					//normalize the slide direction by mulitplying it to the slide speed (Rebecka)
					FVector SlideDirection = PlayerVelocity.GetSafeNormal() * SlideSpeed;

					//apply slide velocity to the character
					GetCharacterMovement()->Launch(SlideDirection);
					GetCharacterMovement()->SetMovementMode(MOVE_Walking);

					bIsSliding = true;
					LastSlideTime = GetWorld()->GetTimeSeconds();

					//set a timer for how long sliding lasts
					GetWorldTimerManager().SetTimer(SliderTimerHandle, this, &APlayerCharacter::StopSlide, SlideDuration, false);
				}
			}
		}
}

void APlayerCharacter::StopSlide()
{
	//reset any changes made during the slide (Rebecka)
	GetCapsuleComponent()->SetCapsuleHalfHeight(GetDefaultHalfHeight(),true); //restores original capsule height
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	bIsSliding = false;

	//reset the camera position
	if(FPSCamera)
	{
		FVector DefaultCameraPosition = FPSCamera->GetComponentLocation();
		DefaultCameraPosition.Z += SlideCameraOffset; //adding back the offset to restore the original Z position of the camera (Rebecka)
		FPSCamera->SetWorldLocation(DefaultCameraPosition);
	}
}


//method for making damage to a character (Rebecka)
float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToMake = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//to make sure that the DamageToMake is not greater than the health we have left, therefore we make the DamageToMake to be the amount we have left (Rebecka) 
	DamageToMake = FMath::Min(Health,DamageToMake);
	Health -= DamageToMake;
	//log to see how much health is left
	UE_LOG(LogTemp, Warning, TEXT("Health left: %f"), Health);
	return DamageToMake;
}

float APlayerCharacter::GetHealthPercent() const
{
	return Health/MaxHealth;
}

float APlayerCharacter::GetDashCooldownPercentage() const
{
	float RemainingCooldown = FMath::Max(0.0f, LastDashTime + DashCooldown - GetWorld()->GetTimeSeconds());
	return RemainingCooldown/DashCooldown;
}
