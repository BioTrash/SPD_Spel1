#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "EnemyCommunicationManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerLocationUpdatedDelegate, const FVector&, NewPlayerLocation);

UCLASS(Blueprintable, BlueprintType)
class SPD_SPEL1_API UEnemyCommunicationManager : public UObject
{
	GENERATED_BODY()

public:
	UEnemyCommunicationManager();

	// Singleton pattern to access the instance
	static UEnemyCommunicationManager* GetInstance();

	// Get the current player location
	UFUNCTION(BlueprintPure, Category = "Enemy Communication")
	FVector GetPlayerLocation() const;

	// Set the player location and broadcast the update
	UFUNCTION(BlueprintCallable, Category = "Enemy Communication")
	void SetPlayerLocation(const FVector& NewPlayerLocation);

	// Event delegate to notify when player location is updated
	UPROPERTY(BlueprintAssignable, Category = "Enemy Communication")
	FPlayerLocationUpdatedDelegate OnPlayerLocationUpdated;

private:
	// Player location stored in the manager
	FVector PlayerLocation;
    
	// Weak pointer to the singleton instance
	static TWeakObjectPtr<UEnemyCommunicationManager> SingletonInstance;
};