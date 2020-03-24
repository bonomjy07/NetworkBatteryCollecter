// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

//#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Character.h"
#include "BatteryNetCharacter.generated.h"

UCLASS(config=Game)
class ABatteryNetCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;

protected:
	/** Collection Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battery", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollectionSphere;

	/** Gun mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Gun;

	/** Muzzle location for gun */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* MuzzleLocation;

protected:
	virtual void BeginPlay();

public:
	virtual void Tick(float DeltaTime) override;

public:
	ABatteryNetCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns CollectionSphere subobject **/
	FORCEINLINE class USphereComponent* GeCollectionSphere() const { return CollectionSphere; }

public:
	// Required network setup
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Access to the character's starting power
	UFUNCTION(BlueprintPure, Category = "Battery")
	float GetInitialPower();

	// Access to the character's current power
	UFUNCTION(BlueprintPure, Category = "Battery")
	float GetCurrentPower();

	UFUNCTION(BlueprintCallable, Category = "GunPlay")
	float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/**
	* This updates the character's power level
	* @param DeltaPower This is the amount to change  power by, can be + or -
	*/
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Battery")
	void UpdatePower(float DeltaPower);

	UFUNCTION(BlueprintImplementableEvent, Category = "Battery")
	void PowerChangeEffect();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery") 
	float BaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battery") 
	float SpeedFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunPlay")
	float FireRate;

	UFUNCTION(NetMulticast, Reliable)
	void OnPlayerDeath();

	UFUNCTION(BlueprintImplementableEvent)
	void SetCrosshairPosition();

public:
	// Called when player presses a key to collect pickups
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void CollectPickups();

	// Called when player pressed a key to build something 
	UFUNCTION(BlueprintCallable, Category = "Building")
	void BuildSomething();

	// Called for switching the 3 to 1 person or, 1 to 3 person
	UFUNCTION(BlueprintCallable, Category = Camera)
	void SwitchCamera();

	// Called for firing the gun
	UFUNCTION(BlueprintCallable, Category = "GunPlay")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "GunPlay")
	void StopFire();

	// Called for zooming (iron-sight)
	UFUNCTION(BlueprintCallable, Category = "GunPlay")
	void StartZoom();

	UFUNCTION(BlueprintCallable, Category = "GunPlay")
	void StopZoom();

public:
	// Called on server to process the collection of pickups
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCollectPickups();
	virtual void ServerCollectPickups_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBuildSomething();
	virtual void ServerBuildSomething_Implementation();

	UFUNCTION(Server, Reliable)
	void ServerStartZoom();
	virtual void ServerStartZoom_Implementation();

	UFUNCTION(Server, Reliable)
	void ServerStopZoom();
	virtual void ServerStopZoom_Implementation();

	UFUNCTION(Server, Reliable)
	void HandleFire();
	virtual void HandleFire_Implementation();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	TSubclassOf<class AFloor> BuildClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunPlay")
	TSubclassOf<class AMyProjectile> ProjectileClass;

private:
	// The character's starting power level
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Battery", Meta = (AllowPrivateAccess = "true"))
	float InitialPower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battery", meta = (AllowPrivateAccess = "true"))
	float CollectionSphereRadius;

	// The character's current power level during game play
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPower, VisibleAnywhere, Category = "Battery")
	float CurrentPower;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Battery", Meta = (AllowPrivateAccess = "true"))
	float CurrentSpeed;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GunPlay", Meta = (AllowPrivateAccess = "true"))
	float AimPitch;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "GunPlay", meta = (AllowPrivateAccess = "true"))
	bool bIsIronsightOn;

	bool bIsFiringWeapon;

	FTimerHandle FiringTimer;

	UFUNCTION()
	void OnRep_CurrentPower();

private:
	void NormalizeAimPitch();
};

