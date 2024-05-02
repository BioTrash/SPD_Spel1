// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnpoint.generated.h"


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
	
	UPROPERTY(EditAnywhere, Category="Enemy")
	APawn* EnemyToSpawnClass;
	
private:
	UPROPERTY(VisibleAnywhere, Blueprintable, Category="Components")
	class UCapsuleComponent* CapsuleComponent;
	
	UPROPERTY(VisibleAnywhere, Blueprintable, Category="Components")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UArrowComponent* PointDirection;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	class UNiagaraComponent* SpawnEffect;

	UFUNCTION()
	void OnNiagaraSystemFinished();
	
};
