// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "BatteryPickup.generated.h"

/**
 * 
 */
UCLASS()
class BATTERYNET_API ABatteryPickup : public APickup
{
	GENERATED_BODY()

public:
	// Sets default value for instance of this class
	ABatteryPickup();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Server side handling : Being picked up by the child class
	void PickedUpBy(APawn* Pawn) override;

	// Accessor for the battery's power level
	UFUNCTION(BlueprintPure, Category = "Battery")
	float GetBatteryPower();

protected:
	// Set the amount of power the battery holds
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Battery", Meta = (AllowProtectedAccess = "true"))
	float BatteryPower;
};
