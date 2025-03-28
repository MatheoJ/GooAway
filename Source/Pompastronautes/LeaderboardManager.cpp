// Fill out your copyright notice in the Description page of Project Settings.


#include "LeaderboardManager.h"

// LeaderboardManager.cpp
#include "LeaderboardManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"

void ULeaderboardManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Load leaderboard data when the subsystem initializes
    if (!LoadLeaderboard())
    {
        // If loading fails, create a new leaderboard
        CreateNewLeaderboard();
    }
}

void ULeaderboardManager::Deinitialize()
{
    // Save leaderboard data when the subsystem is destroyed
    SaveLeaderboard();
    
    Super::Deinitialize();
}

void ULeaderboardManager::AddLeaderboardEntry(const FString& TeamName, int32 MoneyWon, int32 SlimesKilled, 
                                            int32 ObjectivesSaved, int32 ObjectivesDead)
{
    if (!LeaderboardSaveGame)
    {
        if (!LoadLeaderboard())
        {
            CreateNewLeaderboard();
        }
    }
    // Create a new entry
    FLeaderboardEntry NewEntry(TeamName, MoneyWon, SlimesKilled, ObjectivesSaved, ObjectivesDead);
    
    // Add to the leaderboard
    LeaderboardSaveGame->LeaderboardEntries.Add(NewEntry);
    
    // If we exceed the maximum number of entries, sort and remove the lowest entries
    if (LeaderboardSaveGame->LeaderboardEntries.Num() > LeaderboardSaveGame->MaxEntries)
    {
        // Sort by money won (you can change this criteria)
        LeaderboardSaveGame->LeaderboardEntries.Sort([](const FLeaderboardEntry& A, const FLeaderboardEntry& B) {
            return A.MoneyWon > B.MoneyWon;
        });
        
        // Keep only the top MaxEntries
        LeaderboardSaveGame->LeaderboardEntries.SetNum(LeaderboardSaveGame->MaxEntries);
    }
    
    // Save the updated leaderboard
    SaveLeaderboard();
    
    // Broadcast that the leaderboard has been updated
    OnLeaderboardUpdated.Broadcast();
}

TArray<FLeaderboardEntry> ULeaderboardManager::GetLeaderboardEntries() const
{
    if (LeaderboardSaveGame)
    {
        return LeaderboardSaveGame->LeaderboardEntries;
    }
    
    return TArray<FLeaderboardEntry>();
}

void ULeaderboardManager::ClearLeaderboard()
{
    if (LeaderboardSaveGame)
    {
        LeaderboardSaveGame->LeaderboardEntries.Empty();
        SaveLeaderboard();
        OnLeaderboardUpdated.Broadcast();
    }
}

TArray<FLeaderboardEntry> ULeaderboardManager::GetSortedLeaderboard(bool bByMoney, bool bBySlimes, 
                                                                  bool bByObjectivesSaved, bool bDescending) const
{
    TArray<FLeaderboardEntry> SortedEntries = GetLeaderboardEntries();
    
    // Sort based on the specified criteria
    SortedEntries.Sort([bByMoney, bBySlimes, bByObjectivesSaved, bDescending](const FLeaderboardEntry& A, const FLeaderboardEntry& B) {
        if (bByMoney)
        {
            if (A.MoneyWon != B.MoneyWon)
            {
                return bDescending ? A.MoneyWon > B.MoneyWon : A.MoneyWon < B.MoneyWon;
            }
        }
        
        if (bBySlimes)
        {
            if (A.SlimesKilled != B.SlimesKilled)
            {
                return bDescending ? A.SlimesKilled > B.SlimesKilled : A.SlimesKilled < B.SlimesKilled;
            }
        }
        
        if (bByObjectivesSaved)
        {
            if (A.ObjectivesSaved != B.ObjectivesSaved)
            {
                return bDescending ? A.ObjectivesSaved > B.ObjectivesSaved : A.ObjectivesSaved < B.ObjectivesSaved;
            }
        }
        
        // If all specified criteria are equal, sort by timestamp
        return A.Timestamp > B.Timestamp;
    });
    
    return SortedEntries;
}

bool ULeaderboardManager::LoadLeaderboard()
{
    // Check if the save game exists
    if (UGameplayStatics::DoesSaveGameExist(TEXT("Leaderboard"), 0))
    {
        // Load the save game
        LeaderboardSaveGame = Cast<ULeaderboardSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("Leaderboard"), 0));
        return LeaderboardSaveGame != nullptr;
    }
    
    return false;
}

bool ULeaderboardManager::SaveLeaderboard()
{
    if (LeaderboardSaveGame)
    {
        // Save to disk
        return UGameplayStatics::SaveGameToSlot(LeaderboardSaveGame, LeaderboardSaveGame->SaveSlotName, LeaderboardSaveGame->UserIndex);
    }
    
    return false;
}

void ULeaderboardManager::CreateNewLeaderboard()
{
    // Create a new save game object
    LeaderboardSaveGame = Cast<ULeaderboardSaveGame>(UGameplayStatics::CreateSaveGameObject(ULeaderboardSaveGame::StaticClass()));
}