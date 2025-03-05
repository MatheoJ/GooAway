// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnerManager.h"
#include "SpawnerWave.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawnerManager::ASpawnerManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ASpawnerManager::BeginPlay()
{
	Super::BeginPlay();

	if (bStartSpawningAfterDelay) {
		UE_LOG(LogTemp, Warning, TEXT("Start spawning after delay"));
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpawnerManager::StartSpawning, startDelay, false);
	}	
}

// Called every frame
void ASpawnerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnerManager::StartSpawning()
{
	if (Spawners.Num() == 0) {
		FindSpawners();
	}

	if (Spawners.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("No spawners found"));
		return;
	}

	for (int32 i = 0; i < Spawners.Num(); ++i) {
		UE_LOG(LogTemp, Warning, TEXT("Start spawning"));
		Spawners[i]->StartSpawning();
	}	
}


void ASpawnerManager::FindSpawners()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnerWave::StaticClass(), FoundActors);
	for (int32 i = 0; i < FoundActors.Num(); ++i) {
		ASpawnerWave* Spawner = Cast<ASpawnerWave>(FoundActors[i]);
		Spawners.Add(Spawner);
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawner: %d"), Spawners.Num());

}

