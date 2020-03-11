// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BatteryNetCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Pickup.h"
#include "BatteryPickup.h"

#include "Floor.h"
#include "DrawDebugHelpers.h"
//////////////////////////////////////////////////////////////////////////
// ABatteryNetCharacter

ABatteryNetCharacter::ABatteryNetCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Create the collection shpere 
	CollectionSphereRadius = 200.f;
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(RootComponent);
	CollectionSphere->SetSphereRadius(CollectionSphereRadius);

	InitialPower = 200.f;
	CurrentPower = InitialPower;

	BaseSpeed = 200.f;
	SpeedFactor = 0.25f;

	GetCharacterMovement()->SetIsReplicated(true);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void ABatteryNetCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ABatteryNetCharacter, CollectionSphereRadius);
	DOREPLIFETIME(ABatteryNetCharacter, CurrentSpeed);
	DOREPLIFETIME(ABatteryNetCharacter, InitialPower);
	DOREPLIFETIME(ABatteryNetCharacter, CurrentPower);
}

float ABatteryNetCharacter::GetInitialPower()
{
	return InitialPower;
}

float ABatteryNetCharacter::GetCurrentPower()
{
	return CurrentPower;
}

void ABatteryNetCharacter::UpdatePower(float DeltaPower)
{
	if (Role == ROLE_Authority)	
	{
		CurrentPower += DeltaPower;
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed + CurrentPower*SpeedFactor;
		CurrentSpeed = GetCharacterMovement()->MaxWalkSpeed;
		UE_LOG(LogClass, Warning, TEXT("speed : %f"), GetCharacterMovement()->MaxWalkSpeed);

		// Fake
		OnRep_CurrentPower();
	}
}

void ABatteryNetCharacter::OnPlayerDeath_Implementation()
{
	DetachFromControllerPendingDestroy();

	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);

	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void ABatteryNetCharacter::CollectPickups()
{
	// Ask server to collect pickups
	ServerCollectPickups();
}

bool ABatteryNetCharacter::ServerCollectPickups_Validate()
{
	// Checks bad input, parameters or hacking
	// If something is bad, disconnect client who initiated RPC call
	return true;
}

void ABatteryNetCharacter::ServerCollectPickups_Implementation()
{
	if (Role == ROLE_Authority)
	{
		// Track the total power found in batteries
		float CollectedPower = 0.f;

		// Get all overlapping actors and stroe them in an array
		TArray<AActor*> CollectedActors;
		CollectionSphere->GetOverlappingActors(CollectedActors);

		// Look at each Actor include the CollectionShpere
		for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
		{
			// Add power if we found a battery
			ABatteryPickup* const TestBatteryPickup = Cast<ABatteryPickup>(CollectedActors[iCollected]);
			if (TestBatteryPickup && TestBatteryPickup->IsActive())
			{
				CollectedPower += TestBatteryPickup->GetBatteryPower();
			}

			// Collect the pickup and deactivate it
			APickup* const TestPickup = Cast<APickup>(CollectedActors[iCollected]);
			if (TestPickup && !TestPickup->IsPendingKill() && TestPickup->IsActive())
			{
				TestPickup->PickedUpBy(this);
				TestPickup->SetActivate(false);
			}
		}

		// Update the character's power based on collected power
		if (!FMath::IsNearlyZero(CollectedPower, 0.0001f))
		{
			UpdatePower(CollectedPower);
		}
	}
}

void ABatteryNetCharacter::OnRep_CurrentPower()
{
	PowerChangeEffect();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABatteryNetCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABatteryNetCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABatteryNetCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABatteryNetCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABatteryNetCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABatteryNetCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABatteryNetCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ABatteryNetCharacter::OnResetVR);

	// Handle collection pickups
	PlayerInputComponent->BindAction("Collect", IE_Pressed, this, &ABatteryNetCharacter::CollectPickups);

	PlayerInputComponent->BindAction("Build", IE_Pressed, this, &ABatteryNetCharacter::BuildSomething);
}

void ABatteryNetCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ABatteryNetCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ABatteryNetCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ABatteryNetCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryNetCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryNetCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABatteryNetCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ABatteryNetCharacter::BuildSomething()
{
	ServerBuildSomething();
}

bool ABatteryNetCharacter::ServerBuildSomething_Validate()
{
	// Checks bad input, parameters or hacking
	// If something is bad, disconnect client who initiated RPC call
	return true;
}

void ABatteryNetCharacter::ServerBuildSomething_Implementation()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FHitResult OutHit;
		FVector Start = FollowCamera->GetComponentLocation();
		FVector End = Start + FollowCamera->GetForwardVector() * 1000.f;
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);
		World->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_GameTraceChannel1);

		UPrimitiveComponent* HittedComponent = OutHit.GetComponent();
		if (HittedComponent)
		{
			FVector Location = HittedComponent->GetComponentLocation() - FVector(0, 0, 50);
			World->SpawnActor<AFloor>(BuildClass, Location, FRotator(0, 0, 0));
		}
	}
}
