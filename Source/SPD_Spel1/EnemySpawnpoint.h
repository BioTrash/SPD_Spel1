// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnpoint.generated.h"


class UNiagaraComponent;
class UArrowComponent;

UCLASS()
class SPD_SPEL1_API AEnemySpawnpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnpoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Klassen som kommer spawnas via denna spawnpoint.
	UPROPERTY(EditAnywhere, Category="Enemy")
	UClass* EnemyToSpawnClass;

	UFUNCTION()
	void OnNiagaraSystemFinished(UNiagaraComponent* NiagaraComponent);
	
private:

	UPROPERTY(EditAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BaseMesh;

	// Pil för att visa vart meshen pekar.
	UPROPERTY(EditAnywhere)
	UArrowComponent* ArrowComponent;

	// Effect som spelas innan fiender spawnar.
	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SpawnEffect;

	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotation;
	UPROPERTY()
	UWorld* World;
};
