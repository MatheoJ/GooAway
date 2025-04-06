// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SlimeAiController.generated.h"

/**
 * 
 */
UCLASS()
class POMPASTRONAUTES_API ASlimeAiController : public AAIController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SlimeAI")
	void LaunchSlimeInDirection(FVector Direction, float LaunchForce);
	
};
