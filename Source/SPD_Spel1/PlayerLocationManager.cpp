#include "PlayerLocationManager.h"

UPlayerLocationManager::UPlayerLocationManager()
{
	PlayerLocation = FVector::ZeroVector;
}

UPlayerLocationManager* UPlayerLocationManager::GetInstance()
{
	static UPlayerLocationManager Instance;
	return &Instance;
}

FVector UPlayerLocationManager::GetPlayerLocation() const
{
	return PlayerLocation;
}

void UPlayerLocationManager::SetPlayerLocation(const FVector& NewPlayerLocation)
{
	PlayerLocation = NewPlayerLocation;
}
