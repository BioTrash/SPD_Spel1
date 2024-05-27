#include "GenericObjectPool.h"

// Sets default values
AGenericObjectPool::AGenericObjectPool()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGenericObjectPool::BeginPlay()
{
	Super::BeginPlay();
	InitializeAllPools();
}

// Called every frame
void AGenericObjectPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Define InitializeAllPools here
void AGenericObjectPool::InitializeAllPools()
{
	if (ObjectClassesToPool.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitializeAllPools: No object classes to pool"));
		return;
	}

	for (const FObjectPoolInfo& PoolInfo : ObjectClassesToPool)
	{
		if (PoolInfo.ObjectClass)
		{
			InitializePool(PoolInfo.ObjectClass, PoolInfo.PoolSize);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InitializeAllPools: Found null object class in ObjectClassesToPool"));
		}
	}
}
