// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorWidget.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"

UDamageIndicatorWidget::UDamageIndicatorWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDamageIndicatorWidget::NativeConstruct()
{
	// Call the blueprint "Event Construct" event
	Super::NativeConstruct();
}

void UDamageIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// Make sure to call base's tick function
	Super::NativeTick(MyGeometry, InDeltaTime);

	UWorld* World = GetWorld();

	auto Controller = World->GetFirstPlayerController();
	AActor* Actor = Controller->GetPawn();

	// Forward vector
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector2D Direction2D(Direction);

	FVector ActorLocation = Actor->GetActorLocation();
	FVector CauserLocation = DamageCauserLocation - ActorLocation;
	CauserLocation.Normalize();
	FVector2D DamageCauserLocation2D(CauserLocation);

	UKismetMathLibrary::Atan2(FVector2D::CrossProduct(Direction2D, DamageCauserLocation2D), 
		FVector::DotProduct(Direction, CauserLocation));
}

void UDamageIndicatorWidget::SetDamageCauserLocation(const FVector& CauseLocation)
{
	DamageCauserLocation = CauseLocation;
}
