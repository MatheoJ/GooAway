// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	static FName GetClosestSocketToLocation(AActor* Actor, FVector TargetLocation);
	
};
