#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericObjectPool.generated.h"

class AProjectile;

USTRUCT(BlueprintType)
struct FObjectPoolInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool")
    TSubclassOf<AActor> ObjectClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool")
    int32 PoolSize;
};

UCLASS()
class SPD_SPEL1_API AGenericObjectPool : public AActor
{
    GENERATED_BODY()
    
public:    
    AGenericObjectPool();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;

    template <typename T>
    T* GetPooledObject(TSubclassOf<T> PoolClass);

    template <typename T>
    void ReturnPooledObject(T* Object);

    template <typename T>
    void InitializePool(TSubclassOf<T> ObjectClass, int32 PoolSize);

    UFUNCTION(BlueprintCallable, Category = "Pool")
    AActor* GetPooledActor(TSubclassOf<AActor> PoolClass);

    UFUNCTION(BlueprintCallable, Category = "Pool")
    void ReturnPooledActor(AActor* Object);

private:
    TMap<UClass*, TArray<AActor*>> ObjectPools;

    UPROPERTY(EditAnywhere, Category = "Pool")
    TArray<FObjectPoolInfo> ObjectClassesToPool;

    void InitializeAllPools();
};

template <typename T>
void AGenericObjectPool::InitializePool(TSubclassOf<T> ObjectClass, int32 PoolSize)
{
    if (!ObjectClass)
    {
        UE_LOG(LogTemp, Error, TEXT("InitializePool: ObjectClass is null"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("InitializePool: World is null"));
        return;
    }

    TArray<AActor*>& Pool = ObjectPools.FindOrAdd(*ObjectClass);
    for (int32 i = 0; i < PoolSize; i++)
    {
        T* Object = World->SpawnActor<T>(ObjectClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (Object)
        {
            Object->SetActorHiddenInGame(true);
            Object->SetActorEnableCollision(false);
            Object->SetActorTickEnabled(false);
            Pool.Add(Object);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("InitializePool: Failed to spawn object of class %s"), *ObjectClass->GetName());
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("POOL : %s initialized with %d objects"), *ObjectClass->GetName(), PoolSize);
}

template <typename T>
T* AGenericObjectPool::GetPooledObject(TSubclassOf<T> PoolClass)
{
    TArray<AActor*>* Pool = ObjectPools.Find(PoolClass);
    if (Pool)
    {
        for (AActor* Object : *Pool)
        {
            if (!Object->IsActorTickEnabled())
            {
                return Cast<T>(Object);
            }
        }
    }
    return nullptr;
}

template <typename T>
void AGenericObjectPool::ReturnPooledObject(T* Object)
{
    if (Object)
    {
        Object->SetActorHiddenInGame(true);
        Object->SetActorEnableCollision(false);
        Object->SetActorTickEnabled(false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ReturnPooledObject: Object is null"));
    }
}
