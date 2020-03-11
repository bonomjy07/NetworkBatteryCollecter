// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floor.generated.h"

UCLASS()
class BATTERYNET_API AFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building", Meta = (BlueprintProtected = "true"))
	class USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building", Meta = (BlueprintProtected = "true"))
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building", Meta = (BlueprintProtected = "true"))
	class UBoxComponent* BoxCollision1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building", Meta = (BlueprintProtected = "true"))
	class UBoxComponent* BoxCollision2;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building", Meta = (BlueprintProtected = "true"))
	class UBoxComponent* BoxCollision3;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building", Meta = (BlueprintProtected = "true"))
	class UBoxComponent* BoxCollision4;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building", Meta = (BlueprintProtected = "true"))
	class UBoxComponent* BoxCollision5;

private:
	TArray<class UBoxComponent*> BoxCollisions;
};
