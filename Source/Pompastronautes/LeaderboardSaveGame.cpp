// Fill out your copyright notice in the Description page of Project Settings.

// LeaderboardSaveGame.cpp
#include "LeaderboardSaveGame.h"

ULeaderboardSaveGame::ULeaderboardSaveGame()
{
	// Default values
	SaveSlotName = TEXT("Leaderboard");
	UserIndex = 0;
	MaxEntries = 100; // Store up to 100 entries by default
}