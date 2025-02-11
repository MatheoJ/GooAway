// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bShouldSpawn)
		return;

	// Update timer and check for spawn for each slime type
	for (FSlimeSpawnInfo& SpawnInfo : SlimeTypes)
	{
		if (!SpawnInfo.SlimeClass)
			continue;

		SpawnInfo.TimeSinceLastSpawn += DeltaTime;
        
		if (SpawnInfo.TimeSinceLastSpawn >= SpawnInfo.SpawnRate)
		{
			SpawnSlime(SpawnInfo.SlimeClass);
			SpawnInfo.TimeSinceLastSpawn = 0.0f;
		}
	}
}

void ASpawner::SpawnSlime(TSubclassOf<APawn> SlimeClass)
{
	if (!GetWorld() || !SlimeClass)
		return;

	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
    
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
	GetWorld()->SpawnActor<APawn>(SlimeClass, SpawnLocation, SpawnRotation, SpawnParams);
}


