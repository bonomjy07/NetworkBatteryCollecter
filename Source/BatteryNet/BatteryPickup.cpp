// Fill out your copyright notice in the Description page of Project Settings.


#include "BatteryPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ABatteryPickup::ABatteryPickup()
{
	// Keep movement synced from server to client
	bReplicateMovement = true;

	// This pickup is physics enabled and should move
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetMobility(EComponentMobility::Movable);

	// Set a base value for the power
	BatteryPower = 50.f;
}

void ABatteryPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABatteryPickup, BatteryPower);
}

void ABatteryPickup::PickedUpBy(APawn* Pawn)
{
	Super::PickedUpBy(Pawn);
	if (Role == ROLE_Authority)
	{
		// Give clients time to player vfx, etc... before destroying the battery
		SetLifeSpan(2.f);
	}
}

float ABatteryPickup::GetBatteryPower()
{
	return BatteryPower;
}
