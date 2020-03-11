// Fill out your copyright notice in the Description page of Project Settings.


#include "BatteryGameStateBase.h"
#include "Net/UnrealNetwork.h"

ABatteryGameStateBase::ABatteryGameStateBase()
{
	CurrentState = EBatteryGameState::EUnknown;
}

void ABatteryGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABatteryGameStateBase, PowerToWin);
	DOREPLIFETIME(ABatteryGameStateBase, CurrentState);
	DOREPLIFETIME(ABatteryGameStateBase, WinningPlayerName);
}

EBatteryGameState ABatteryGameStateBase::GetCurrentGameState() const
{
	return CurrentState;
}

void ABatteryGameStateBase::SetCurrentGameState(enum EBatteryGameState NewGameState) 
{
	if (Role == ROLE_Authority)
	{
		CurrentState = NewGameState;
	}
}

void ABatteryGameStateBase::OnRep_CurrentState()
{
}
