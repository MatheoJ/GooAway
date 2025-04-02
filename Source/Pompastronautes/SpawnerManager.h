// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnerManager.generated.h"

UCLASS()
class POMPASTRONAUTES_API ASpawnerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bStartSpawningAfterDelay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (EditCondition = "bStartSpawningAfterDelay"))
	float startDelay = 5.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StartSpawning();

	UFUNCTION(BlueprintCallable)
	void StopSpawning();

	UFUNCTION(BlueprintCallable)
	void BeginSpawning();
	
private:
	void FindSpawners();
	
	TArray<class ASpawnerWave*> Spawners;

};
