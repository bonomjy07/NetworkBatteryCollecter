// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTERYNET_API UDamageIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

	/** Location of the pawn who damaged my charcter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material", meta = (AllowPrivateAccess = "true"))
	FVector DamageCauserLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* Material;

public:
	UDamageIndicatorWidget(const FObjectInitializer& ObjectInitializer);

	/** Override the Blueprint "Event Construct" event */
	virtual void NativeConstruct() override;

	/** Override the Blueprint "Event Tick" event */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	/** Return the damage causer location */
	FORCEINLINE const FVector& GetDamageCauserLocation() const { return DamageCauserLocation; };

public:
	void SetDamageCauserLocation(const FVector& CauseLocation);
};
