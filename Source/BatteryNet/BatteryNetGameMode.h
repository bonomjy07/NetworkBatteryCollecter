// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BatteryGameStateBase.h"
#include "BatteryNetGameMode.generated.h"

UCLASS(minimalapi)
class ABatteryNetGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABatteryNetGameMode();

	virtual void BeginPlay() override;

	/** Returns power needed to win - needed for the HUD */
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetPowerToWinMultiplier() const;

	// How many times per second to update characters' power and check game rule
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Power")
	float PowerDrainDelay;

	FTimerHandle PowerDrainTimer;

	int32 DeadPlayerCount;

protected:
	/** The rate at which the chracter loses power */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float DecayRate;

	/** The power needed to win the game */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float PowerToWinMultiplier;

private:
	// Drains power from characters and update gameplay
	void DrainPowerOverTime();

	TArray<class ASpawnVolume*> SpawnVolumeActors;

	void HandleNewState(EBatteryGameState NewState);
};

