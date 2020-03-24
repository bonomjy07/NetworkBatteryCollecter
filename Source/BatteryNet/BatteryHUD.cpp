// Fill out your copyright notice in the Description page of Project Settings.


#include "BatteryHUD.h"
#include "Engine/Canvas.h"


void ABatteryHUD::DrawHUD()
{
	Super::DrawHUD();

	/* Crosshair will be implemented by widget, not HUD */
	return;
	if (CrosshairTexture)
	{
		// Find the center of our canvas
		FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * .5f);

		// Offset by half of the texture's dimension so that the centher of the texture aligns with the center of the canvas
		FVector2D CrosshairDrawPosition(Center.X - (CrosshairTexture->GetSurfaceWidth() * 0.5f),
			Center.Y - (CrosshairTexture->GetSurfaceHeight() * 0.5f));

		// Draw the croshair at the centerpoint
		FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTexture->Resource, FLinearColor::White);
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);
	}
}
