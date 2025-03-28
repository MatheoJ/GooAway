// Fill out your copyright notice in the Description page of Project Settings.

// LeaderboardManager.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LeaderboardSaveGame.h"
#include "LeaderboardManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeaderboardUpdated);

/**
 * Subsystem that manages the game's leaderboard
 */
UCLASS()
class POMPASTRONAUTES_API ULeaderboardManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Begin USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem interface

	// Add a new entry to the leaderboard
	UFUNCTION(BlueprintCallable, Category = "Leaderboard")
	void AddLeaderboardEntry(const FString& TeamName, int32 MoneyWon, int32 SlimesKilled, 
							int32 ObjectivesSaved, int32 ObjectivesDead);

	// Get all leaderboard entries
	UFUNCTION(BlueprintCallable, Category = "Leaderboard")
	TArray<FLeaderboardEntry> GetLeaderboardEntries() const;

	// Clear all leaderboard entries
	UFUNCTION(BlueprintCallable, Category = "Leaderboard")
	void ClearLeaderboard();

	// Sort leaderboard by a specific criteria
	UFUNCTION(BlueprintCallable, Category = "Leaderboard")
	TArray<FLeaderboardEntry> GetSortedLeaderboard(bool bByMoney = true, bool bBySlimes = false, 
												  bool bByObjectivesSaved = false, bool bDescending = true) const;

	// Event called when the leaderboard is updated
	UPROPERTY(BlueprintAssignable, Category = "Leaderboard")
	FOnLeaderboardUpdated OnLeaderboardUpdated;

private:
	// The current loaded leaderboard data
	UPROPERTY()
	ULeaderboardSaveGame* LeaderboardSaveGame;

	// Load leaderboard data
	bool LoadLeaderboard();

	// Save leaderboard data
	bool SaveLeaderboard();

	// Create a new leaderboard save game
	void CreateNewLeaderboard();
};