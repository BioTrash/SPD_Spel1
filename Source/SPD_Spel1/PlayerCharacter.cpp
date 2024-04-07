// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

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

	// Checks whether player is suppose to have any weapons at the start or not (Rufus)
	if(!InitialWeaponArray.IsEmpty())
	{
		// Goes through the entire array of weapon blueprints (Rufus)
		for(TSubclassOf<AActor>& Weapon : InitialWeaponArray) 
		{
			// Spawns in each weapon blue print at zero world position (Rufus)
			AActor* WeaponInstance = GetWorld()->SpawnActor<AActor>(*Weapon, FVector::ZeroVector, FRotator::ZeroRotator);

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

				// Helps with positioning, closer to the actual bone (Rufus)
				WeaponInstance->SetOwner(this);
				
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

	PlayerInputComponent->BindAction(TEXT("Dash"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Dash);

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
		AActor* lastElement = CurrentWeaponArray.Last(); // Save the last element
    
		// Shift elements to the right
		for (int i = CurrentWeaponArray.Num() - 1; i > 0; --i) {
			CurrentWeaponArray[0]->SetActorHiddenInGame(true);
			CurrentWeaponArray[i] = CurrentWeaponArray[i - 1];
			
		}

		// Move the last element to the beginning
		CurrentWeaponArray[0] = lastElement;
		CurrentWeaponArray[0]->SetActorHiddenInGame(false);
		
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
			//normalize the dash direction and multiply it by dash speed
			FVector DashDirection = GetActorForwardVector().GetSafeNormal() * DashSpeed;

			//apply dash velocity to the character
			GetCharacterMovement()->Launch(DashDirection);

			bIsDashing = true;
			LastDashTime = GetWorld()->GetTimeSeconds();

			//reset dash after duration
			GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacter::StopDash, DashDuration, false);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Character movement component not found!"));
		}
	}
}

//method for stopdashing (Rebecka)
void APlayerCharacter::StopDash()
{
	bIsDashing = false;
}



