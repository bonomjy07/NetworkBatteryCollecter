// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	if (Role == ROLE_Authority)
	{
		bIsActive = true;
	}

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetGenerateOverlapEvents(true);
	RootComponent = Cast<USceneComponent>(PickupMesh);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickup, bIsActive);
}

UStaticMeshComponent* APickup::GetMesh() const
{
	return PickupMesh;
}

// Return active state
bool APickup::IsActive() const
{
	return bIsActive;
}

// Set new pickup state
void APickup::SetActivate(bool NewPickupState)
{
	if (Role == ROLE_Authority)
	{
		bIsActive = NewPickupState;
	}
}

void APickup::WasCollected_Implementation()
{
	// Log a debug message
	FString PickupDebugString = GetName();
	UE_LOG(LogClass, Warning, TEXT("You have collected %s"), *PickupDebugString);
}

void APickup::PickedUpBy(APawn* Pawn)
{
	if (Role == ROLE_Authority)
	{
		// Store thee pawn who picked up the pickup
		PickupInstigator = Pawn;
		// Notify clients of the picked up action
		ClientOnPickedUpBy(Pawn);
	}
}

void APickup::ClientOnPickedUpBy_Implementation(APawn* Pawn)
{
	// Store the pawn who picked up the pickup (client)
	PickupInstigator = Pawn;
	// Fire the blueprint native event, which cannot be replicated
	WasCollected();
}
