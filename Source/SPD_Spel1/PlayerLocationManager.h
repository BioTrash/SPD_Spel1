// PlayerLocationManager.h

#pragma once

#include "CoreMinimal.h"

class PLAYERLOCATIONMANAGER_API UPlayerLocationManager
{
public:
	UPlayerLocationManager();

	static UPlayerLocationManager* GetInstance();

	FVector GetPlayerLocation() const;
	void SetPlayerLocation(const FVector& NewPlayerLocation);

private:
	FVector PlayerLocation;
};
