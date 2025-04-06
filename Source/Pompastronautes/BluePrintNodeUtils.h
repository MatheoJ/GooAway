// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OilSpill.h"
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
	
};
