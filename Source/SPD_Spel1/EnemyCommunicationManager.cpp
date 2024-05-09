#include "EnemyCommunicationManager.h"

TWeakObjectPtr<UEnemyCommunicationManager> UEnemyCommunicationManager::SingletonInstance = nullptr;

UEnemyCommunicationManager::UEnemyCommunicationManager()
{
	PlayerLocation = FVector::ZeroVector;
	UE_LOG(LogTemp, Error, TEXT("COMMUNICATION MANAGER ACTIVE"));
}

UEnemyCommunicationManager* UEnemyCommunicationManager::GetInstance()
{
	if (SingletonInstance.IsValid())
	{
		return SingletonInstance.Get();
	}

	SingletonInstance = NewObject<UEnemyCommunicationManager>();
	return SingletonInstance.Get();
}

FVector UEnemyCommunicationManager::GetPlayerLocation() const
{
	return PlayerLocation;
}

void UEnemyCommunicationManager::SetPlayerLocation(const FVector& NewPlayerLocation)
{
	PlayerLocation = NewPlayerLocation;
	UE_LOG(LogTemp, Log, TEXT("Player location updated: %s"), *PlayerLocation.ToString());
	// Broadcast the player location update event
	OnPlayerLocationUpdated.Broadcast(PlayerLocation);
}