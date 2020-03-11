// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class BATTERYNET_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Find random point withn the box component
	UFUNCTION(BlueprintPure, Category = "Spawn")
	FVector GetRandomPointInVolume();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SetSpawningActive(bool bShouldSpawn);

protected:
	// This is the pickup to spawn
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class APickup> WhatToSpawn;

	// Timer for recurring spawning
	FTimerHandle SpawnTimer;

	// Minimun spawn deley
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Spawn")
	float SpawnRangeLow;

	// Maximum spawn deley
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Spawn")
	float SpawnRangeHigh;

private:
	// The spawn area where pickups will be created
	UPROPERTY(VisibleAnywhere, Category = "Spawn", Meta = (AllowPrivatedAccess = "true"))
	class UBoxComponent* WhereToSpawn;

	// Handles spawning a new pickup
	void SpawnPickup();

	// Acutial spawn delay
	float SpawnDelay;
};
