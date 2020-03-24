// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class BATTERYNET_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Required network setup
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Return the mesh for pickup. */
	UStaticMeshComponent* GetMesh() const;

	/** Return whether or not the pickup is active */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool IsActive() const;

	/** Allows other classes to safely change whether or not */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetActivate(bool NewPickupState);

	/**Function that called when pickup is collected */
	UFUNCTION(BlueprintNativeEvent)
	void WasCollected();
	virtual void WasCollected_Implementation();

	// Server side handling : Being picked up
	UFUNCTION(BlueprintAuthorityOnly, Category = "Pickup")
	virtual void PickedUpBy(APawn* Pawn);

protected:
	/** True when the pickup can be used, and false when pickup is deactivated.*/
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Pickup", Meta = (BlueprintProtected = "true"))
	bool bIsActive;

	// The pawn who picked up the pickup
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Pickup", Meta = (BlueprintProtected = "true"))
	APawn* PickupInstigator;
	/*
	UFUNCTION()
	virtual void OnRep_IsActive();
	*/

private:
	/** Static mesh to represnta the pickup in the level.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PickupMesh;

	// Client side handling : being picked up
	UFUNCTION(NetMulticast, Unreliable)
	void ClientOnPickedUpBy(APawn* Pawn);
};
