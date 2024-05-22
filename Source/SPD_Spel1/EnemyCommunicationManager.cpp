#include "EnemyCommunicationManager.h"

TWeakObjectPtr<UEnemyCommunicationManager> UEnemyCommunicationManager::SingletonInstance = nullptr;

UEnemyCommunicationManager::UEnemyCommunicationManager()
{
	PlayerLocation = FVector::ZeroVector;
	//UE_LOG(LogTemp, Error, TEXT("COMMUNICATION MANAGER ACTIVE"));
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
	OnPlayerLocationUpdated.Broadcast(PlayerLocation);
}

void UEnemyCommunicationManager::SetIsChasing(const bool& IsChasing)
{
	Chase = IsChasing;
	OnChaseUpdated.Broadcast(Chase);
}

bool UEnemyCommunicationManager::GetSetIsChasing() const
{
	return Chase;
}