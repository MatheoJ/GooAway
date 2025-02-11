// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeNavGridSaveGame.h"

USlimeNavGridSaveGame::USlimeNavGridSaveGame(FString AdditionalString)
{
	SaveSlotName = TEXT("SlimeNavGrid")+ AdditionalString;
	UserIndex = 0;
}

USlimeNavGridSaveGame::USlimeNavGridSaveGame()
{
	SaveSlotName = TEXT("SlimeNavGrid");
	UserIndex = 0;
}
