// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

UCLASS()
class SPD_SPEL1_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bCheckpointActive;
	
	UPROPERTY(EditAnywhere);
	class AActor* Player;

	UPROPERTY(EditAnywhere);
	class AGameModeBase* GameMode;
	
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxCollision;

	// Pil för att visa vart meshen pekar.
	UPROPERTY(EditAnywhere)
	class UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere)
	class UTextRenderComponent* CheckpointText;
	
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
