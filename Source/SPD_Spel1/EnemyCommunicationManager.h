#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "EnemyCommunicationManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerLocationUpdatedDelegate, const FVector&, NewPlayerLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChaseUpdatedDelegate, const bool, Chase);



UCLASS(Blueprintable, BlueprintType)
class SPD_SPEL1_API UEnemyCommunicationManager : public UObject
{
	GENERATED_BODY()

public:
	UEnemyCommunicationManager();

	// Get-metod för instans
	static UEnemyCommunicationManager* GetInstance();

	// Get-metod för PlayerLocation
	UFUNCTION(BlueprintPure, Category = "Enemy Communication")
	FVector GetPlayerLocation() const;
	
	//Get-metod för Chase
	UFUNCTION(BlueprintPure, Category = "Enemy Communication")
	bool GetSetIsChasing() const;

	// Set-metod för Location och broadcasta denna
	UFUNCTION(BlueprintCallable, Category = "Enemy Communication")
	void SetPlayerLocation(const FVector& NewPlayerLocation);

	// Set-metod för Chase och broadcasta denna
	UFUNCTION(BlueprintCallable, Category = "Enemy Communication")
	void SetIsChasing(const bool& IsChasing);
	
	// Event-delegat: Notifierar när PlayerLocation ändrats
	UPROPERTY(BlueprintAssignable, Category = "Enemy Communication")
	FPlayerLocationUpdatedDelegate OnPlayerLocationUpdated;

	// Event-delegat: Notifierar när Chase ändrats
	UPROPERTY(BlueprintAssignable, Category = "Enemy Communication")
	FChaseUpdatedDelegate OnChaseUpdated;

private:
	// Spelarens location
	FVector PlayerLocation;
	
	// Chase-variabeln
	bool Chase;
    
	// Singleton-instansen
	static TWeakObjectPtr<UEnemyCommunicationManager> SingletonInstance;
};