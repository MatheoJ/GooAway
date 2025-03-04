// SpawnerWave.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlimeBase.h"
#include "SpawnerWave.generated.h"

USTRUCT(BlueprintType)
struct FSlimeWaveStep
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TSubclassOf<class ASlimeBase> SlimeClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    int NumberOfSlimesToSpawn = 1;

    // Track how many slimes have been spawned for this step
    int SpawnedCount = 0;
};

USTRUCT(BlueprintType)
struct FSlimeWave
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TArray<FSlimeWaveStep> WaveSteps;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    float SpawnRate = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    float TimeToWaitAfterPreviousWave = 1.0f;

    float TimeSinceLastSpawn = 0.0f;
    
    // Track which step we're on
    int CurrentStepIndex = 0;
    
    // Track if the wave is complete
    bool IsComplete() const 
    {
        return CurrentStepIndex >= WaveSteps.Num();
    }
    
    // Reset wave for reuse
    void Reset()
    {
        CurrentStepIndex = 0;
        TimeSinceLastSpawn = 0.0f;
        
        for (auto& Step : WaveSteps)
        {
            Step.SpawnedCount = 0;
        }
    }
};

UCLASS()
class POMPASTRONAUTES_API ASpawnerWave : public AActor
{
    GENERATED_BODY()
    
public: 
    // Sets default values for this actor's properties
    ASpawnerWave();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public: 
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Waves of slimes to spawn (in order)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    TArray<FSlimeWave> Waves;
    
    // Spawn point location
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
    USceneComponent* SpawnPoint;
    
    // Spawn radius (random position within this radius from spawn point)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    float SpawnRadius = 20.0f;
    
    // Slime type to apply to spawned slimes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    ESlimeType DefaultSlimeType = ESlimeType::Water;
    
    // Start spawning waves
    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void StartSpawning();
    
    // Stop spawning
    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void StopSpawning();
    
    // Is currently spawning?
    UFUNCTION(BlueprintPure, Category = "Spawn")
    bool IsSpawning() const { return bIsSpawning; }
    
    // Get current wave index
    UFUNCTION(BlueprintPure, Category = "Spawn")
    int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }
    
private:
    bool bIsSpawning = false;
    int32 CurrentWaveIndex = 0;
    float TimeWaitingForNextWave = 0.0f;
    bool bWaitingForNextWave = false;
    
    // Spawn a slime at the spawn point
    ASlimeBase* SpawnSlime(TSubclassOf<ASlimeBase> const SlimeClass) const;
    
    // Advance to the next wave
    void AdvanceToNextWave();
    
    // Process the current wave
    void ProcessCurrentWave(float DeltaTime);
};