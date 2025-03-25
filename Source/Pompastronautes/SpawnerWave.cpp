// SpawnerWave.cpp
#include "SpawnerWave.h"
#include "Engine/World.h"

ASpawnerWave::ASpawnerWave()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Create spawn point component
    SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
    RootComponent = SpawnPoint;
}

void ASpawnerWave::BeginPlay()
{
    Super::BeginPlay();
    
    //StartSpawning();
}

void ASpawnerWave::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (!bIsSpawning || Waves.Num() == 0)
    {
        return;
    }
    
    if (bWaitingForNextWave)
    {
        // We're waiting for the next wave to start
        TimeWaitingForNextWave += DeltaTime;
        
        // Check if it's time to start the next wave
        if (TimeWaitingForNextWave >= Waves[CurrentWaveIndex].TimeToWaitAfterPreviousWave)
        {
            bWaitingForNextWave = false;
            TimeWaitingForNextWave = 0.0f;
        }
        else
        {
            // Keep waiting
            return;
        }
    }
    
    // Process the current wave
    ProcessCurrentWave(DeltaTime);
}

void ASpawnerWave::StartSpawning()
{
    if (Waves.Num() == 0)
    {
        return;
    }
    
    // Reset all waves
    for (auto& Wave : Waves)
    {
        Wave.Reset();
    }
    
    CurrentWaveIndex = 0;
    bIsSpawning = true;
    bWaitingForNextWave = true;
    TimeWaitingForNextWave = 0.0f;
}

void ASpawnerWave::StopSpawning()
{
    bIsSpawning = false;
}

ASlimeBase* ASpawnerWave::SpawnSlime(TSubclassOf<ASlimeBase> const SlimeClass) 
{
    if (!SlimeClass)
    {
        return nullptr;
    }
    
    // Generate random position within radius
    FVector SpawnLocation = SpawnPoint->GetComponentLocation();
    if (SpawnRadius > 0.0f)
    {
        // Random 2D offset within spawn radius
        FVector2D RandomOffset = FMath::RandPointInCircle(SpawnRadius);
        SpawnLocation.X += RandomOffset.X;
        SpawnLocation.Y += RandomOffset.Y;
    }
    
    // Spawn parameters
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    TotalSlimesSpawned++;

    ASlimeBase* NewSlime = GetWorld()->SpawnActor<ASlimeBase>(SlimeClass, SpawnLocation, SpawnPoint->GetComponentRotation(), SpawnParams);
    return NewSlime;
}

void ASpawnerWave::AdvanceToNextWave()
{
    CurrentWaveIndex++;
    
    // Check if we've finished all waves
    if (CurrentWaveIndex >= Waves.Num())
    {
        // All waves complete
        StopSpawning();
    }
    else
    {
        // Wait for the next wave to start
        bWaitingForNextWave = true;
        TimeWaitingForNextWave = 0.0f;
    }
}

void ASpawnerWave::ProcessCurrentWave(float DeltaTime)
{
    if (!Waves.IsValidIndex(CurrentWaveIndex))
    {
        StopSpawning();
        return;
    }
    
    FSlimeWave& CurrentWave = Waves[CurrentWaveIndex];
    
    // Check if this wave is complete
    if (CurrentWave.IsComplete())
    {
        AdvanceToNextWave();
        return;
    }
    
    // Increment timer
    CurrentWave.TimeSinceLastSpawn += DeltaTime;
    
    // Time to spawn?
    if (CurrentWave.TimeSinceLastSpawn >= CurrentWave.SpawnRate)
    {
        CurrentWave.TimeSinceLastSpawn = 0.0f;
        
        // Get current step
        if (!CurrentWave.WaveSteps.IsValidIndex(CurrentWave.CurrentStepIndex))
        {
            // Invalid step, move to next wave
            AdvanceToNextWave();
            return;
        }
        
        FSlimeWaveStep& CurrentStep = CurrentWave.WaveSteps[CurrentWave.CurrentStepIndex];
        
        // Spawn a slime for this step
        SpawnSlime(CurrentStep.SlimeClass);
        
        // Increment spawn count
        CurrentStep.SpawnedCount++;
        
        // Check if this step is complete
        if (CurrentStep.SpawnedCount >= CurrentStep.NumberOfSlimesToSpawn)
        {
            // Move to next step
            CurrentWave.CurrentStepIndex++;
        }
    }
}