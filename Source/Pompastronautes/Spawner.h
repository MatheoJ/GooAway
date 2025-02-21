// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"



USTRUCT(BlueprintType)
struct FSlimeSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<class APawn> SlimeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnRate = 5.0f;

	float TimeSinceLastSpawn = 0.0f;
};


UCLASS()
class POMPASTRONAUTES_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<FSlimeSpawnInfo> SlimeTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bShouldSpawn = true;

private:
	UFUNCTION()
	void SpawnSlime(TSubclassOf<APawn> SlimeClass);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
