// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BatteryGameStateBase.generated.h"

UENUM(BlueprintType)
enum EBatteryGameState
{
	EPlaying,
	EGameOver,
	EWon,
	EUnknown
};

/**
 * 
 */
UCLASS()
class BATTERYNET_API ABatteryGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ABatteryGameStateBase();

	// Required network setup
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Track the power level required to win the game
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Power")
	float PowerToWin;
	
	// Retruns the current state of gameplay
	UFUNCTION(BlueprintPure, Category = "Power")
	EBatteryGameState GetCurrentGameState() const;

	// Transition the game to a new play state. only usable on server
	void SetCurrentGameState(enum EBatteryGameState NewGameState);

	// Rep Notify fired on clients to allow for client-side changes based on changes in gameplay state
	UFUNCTION()
	void OnRep_CurrentState();

	// The name of player who won the game
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Power")
	FString WinningPlayerName;

private:
	// Track the current playing state
	UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
	TEnumAsByte<enum EBatteryGameState> CurrentState;
};
