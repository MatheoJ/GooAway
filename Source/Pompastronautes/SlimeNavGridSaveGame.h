// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SlimeNavGridSaveGame.generated.h"

/** Describes relations between navigation points*/
USTRUCT()
struct FSlimeNavRelations
{
	GENERATED_BODY()

	/** Array of indexes of neighboring navigtaion points*/
	UPROPERTY()
	TArray<int32> Neighbors;
};

/**
 * 
 */
UCLASS()
class POMPASTRONAUTES_API USlimeNavGridSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** Locations of navigation points */
	UPROPERTY()
	TMap<int32, FVector> NavLocations;

	/** Normals of navigation points */
	UPROPERTY()
	TMap<int32, FVector> NavNormals;

	/** Relations between navigation points */
	UPROPERTY()
	TMap<int32, FSlimeNavRelations> NavRelations;

	/** Name of save slot to store navigation grid */
	UPROPERTY()
	FString SaveSlotName;

	/** UserIndex to store navigation grid */
	UPROPERTY()
	uint32 UserIndex;

	USlimeNavGridSaveGame();	
};
