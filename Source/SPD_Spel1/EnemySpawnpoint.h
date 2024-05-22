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
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Enemy")
	TSubclassOf<AActor> EnemyToSpawnClass;

	// Effect som spelas innan fiender spawnar.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraComponent* SpawnEffect;
	
	UFUNCTION()
	void OnNiagaraSystemFinished(UNiagaraComponent* NiagaraComponent);
	
	
private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	// Pil för att visa vart meshen pekar.
	UPROPERTY(VisibleAnywhere)
	UArrowComponent* ArrowComponent;

	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotation;
	UPROPERTY()
	UWorld* World;
};
