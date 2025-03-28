// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LeaderboardSaveGame.generated.h"

// Structure to hold each leaderboard entry
USTRUCT(BlueprintType)
struct FLeaderboardEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
    FString TeamName;

    UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
    int32 MoneyWon;

    UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
    int32 SlimesKilled;

    UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
    int32 ObjectivesSaved;

    UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
    int32 ObjectivesDead;

    // Add a timestamp for sorting entries (optional)
    UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
    FDateTime Timestamp;

    // Default constructor
    FLeaderboardEntry()
    {
        TeamName = TEXT("");
        MoneyWon = 0;
        SlimesKilled = 0;
        ObjectivesSaved = 0;
        ObjectivesDead = 0;
        Timestamp = FDateTime::Now();
    }

    // Custom constructor
    FLeaderboardEntry(const FString& InTeamName, int32 InMoneyWon, int32 InSlimesKilled, 
                    int32 InObjectivesSaved, int32 InObjectivesDead)
        : TeamName(InTeamName)
        , MoneyWon(InMoneyWon)
        , SlimesKilled(InSlimesKilled)
        , ObjectivesSaved(InObjectivesSaved)
        , ObjectivesDead(InObjectivesDead)
        , Timestamp(FDateTime::Now())
    {
    }
};

// The SaveGame class that will store all leaderboard entries
UCLASS()
class POMPASTRONAUTES_API ULeaderboardSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    // Constructor
    ULeaderboardSaveGame();

    // The array that stores all leaderboard entries
    UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
    TArray<FLeaderboardEntry> LeaderboardEntries;

    // Save slot name
    UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
    FString SaveSlotName;

    // User index
    UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
    int32 UserIndex;

    // Maximum number of entries to keep
    UPROPERTY(BlueprintReadWrite, Category = "Leaderboard")
    int32 MaxEntries;
};