// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorWidget.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/Border.h"

#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

UDamageIndicatorWidget::UDamageIndicatorWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//SetAnchorsInViewport(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
	//	SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
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

	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Character)
	{
		FVector V1(Character->GetActorForwardVector());
		FVector V2(Character->GetActorLocation() - DamageCauserLocation);
		V2.Normalize();

		float Y = FVector::CrossProduct(V1, V2).Z;
		float X = FVector::DotProduct(V1, V2);

		float Angle = UKismetMathLibrary::DegAtan2(Y, X); // Degree
		float ClampedAngle = UKismetMathLibrary::MapRangeClamped(Angle, 180.f, -180.f, 0.f, 1.f);

		if (Material)
		{
			Material->SetScalarParameterValue(FName(TEXT("Angle")), ClampedAngle);
		}
	}
}

void UDamageIndicatorWidget::SetDamageCauserLocation(const FVector& CauseLocation)
{
	DamageCauserLocation = CauseLocation;
}
