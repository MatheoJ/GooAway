// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OilSpill.h"
#include "LeaderboardManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BluePrintNodeUtils.generated.h"

/**
 * 
 */
UCLASS()
class POMPASTRONAUTES_API UBluePrintNodeUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static AActor* GetClosestActor(TArray<AActor*> Actors, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static FName GetClosestSocketToLocation(AActor* Actor, FVector TargetLocation, FName SocketsToExclude = NAME_None);

	
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static FVector GetClosestSocketLocation(AActor* Actor, FVector TargetLocation, FName SocketsToExclude = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static bool IsActorInSublevel(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static bool ArePlayersBehindPlane(const TArray<ACharacter*>& Players, const FVector& PlanePosition, const FVector& PlaneNormal);

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static AOilSpill* GetClosestOilSpillOnFireInHitArray(const TArray<FHitResult>& HitResults, const FVector& ActorLocation, float MaxDistance = 1000.0f);

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static int GetIndexOfLastLeaderboardEntry(const TArray<FLeaderboardEntry>& LeaderboardEntries);

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static AActor* GetClosestActorSlimeWithTypeExcept(TArray<AActor*> Actors, FVector Location, ESlimeType SlimeTypeToExclude);

	UFUNCTION(BlueprintPure, Category = "Utilities")
	static bool ProjectWorldToScreenBidirectional(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, bool& bTargetBehindCamera, bool bPlayerViewportRelative = false);

	UFUNCTION(BlueprintCallable, Category = "UI|Widgets")
	static FVector2D ProjectWidgetPositionToBorder(const FVector2D& WidgetPosition, const FVector2D& ViewportSize);
	
};
