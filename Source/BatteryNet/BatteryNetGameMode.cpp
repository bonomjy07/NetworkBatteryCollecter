// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BatteryNetGameMode.h"
#include "BatteryNetCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "BatteryGameStateBase.h"
#include "SpawnVolume.h"

ABatteryNetGameMode::ABatteryNetGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	GameStateClass = ABatteryGameStateBase::StaticClass();

	DecayRate = 0.02f;

	PowerDrainDelay = 0.25f;

	PowerToWinMultiplier = 1.25f;

	DeadPlayerCount = 0;

}

void ABatteryNetGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	check(World);
	ABatteryGameStateBase* MyGameState = Cast<ABatteryGameStateBase>(GameState);
	check(GameState);
	MyGameState->PowerToWin = 200.f * PowerToWinMultiplier;

	DeadPlayerCount = 0;

	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsOfClass(World, ASpawnVolume::StaticClass(), FoundActor);

	for (auto* Actor : FoundActor)
	{
		if (ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(Actor))
		{
			SpawnVolumeActors.AddUnique(SpawnVolume);
		}
	}

	HandleNewState(EBatteryGameState::EPlaying);
}

float ABatteryNetGameMode::GetPowerToWinMultiplier() const
{
	return PowerToWinMultiplier;
}

void ABatteryNetGameMode::DrainPowerOverTime()
{
	UWorld* World = GetWorld();
	check(World);
	ABatteryGameStateBase* MyGameState = Cast<ABatteryGameStateBase>(GameState);
	check(GameState);

	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(*It))
		{
			if (ABatteryNetCharacter* BatteryCharacter = Cast<ABatteryNetCharacter>(PlayerController->GetPawn()))
			{
				if (BatteryCharacter->GetCurrentPower() > MyGameState->PowerToWin)
				{
					MyGameState->WinningPlayerName = BatteryCharacter->GetName();
					HandleNewState(EBatteryGameState::EWon);
				}
				else if (BatteryCharacter->GetCurrentPower() > 0)
				{
					float InitialPower = BatteryCharacter->GetInitialPower();
					BatteryCharacter->UpdatePower(-PowerDrainDelay * DecayRate * InitialPower);
				}
				else
				{
					BatteryCharacter->OnPlayerDeath();

					++DeadPlayerCount;
					if (DeadPlayerCount >= GetNumPlayers())
					{
						HandleNewState(EBatteryGameState::EGameOver);
					}
				}
			}
		}
	}
}

void ABatteryNetGameMode::HandleNewState(EBatteryGameState NewState)
{
	UWorld* World = GetWorld();
	check(World);
	ABatteryGameStateBase* MyGameState = Cast<ABatteryGameStateBase>(GameState);
	check(GameState);

	if (NewState != MyGameState->GetCurrentGameState())
	{
		MyGameState->SetCurrentGameState(NewState);

		switch (NewState)
		{
		case EPlaying:
			for (const auto& SpawnVolue : SpawnVolumeActors)
			{
				SpawnVolue->SetSpawningActive(true);
			}
			GetWorldTimerManager().SetTimer(PowerDrainTimer, this, &ABatteryNetGameMode::DrainPowerOverTime, PowerDrainDelay, true);
			break;
		case EGameOver:
			for (const auto& SpawnVolue : SpawnVolumeActors)
			{
				SpawnVolue->SetSpawningActive(false);
			}
			GetWorldTimerManager().ClearTimer(PowerDrainTimer);
			break;
		case EWon:
			for (const auto& SpawnVolue : SpawnVolumeActors)
			{
				SpawnVolue->SetSpawningActive(false);
			}
			GetWorldTimerManager().ClearTimer(PowerDrainTimer);
			break;
		case EUnknown:
			break;
		default:
			break;
		}
	}
}
