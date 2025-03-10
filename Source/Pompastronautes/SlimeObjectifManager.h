// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlimeObjectif.h"
#include "GameFramework/Actor.h"
#include "SlimeObjectifManager.generated.h"

UCLASS()
class POMPASTRONAUTES_API ASlimeObjectifManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlimeObjectifManager();

	UFUNCTION(BlueprintCallable)
	FVector GetAttackPointFromClosestObjectif(AActor* actor);

	UFUNCTION(BlueprintCallable)
	ASlimeObjectif* GetClossestObjectif(AActor* actor);

	UFUNCTION(BlueprintCallable, Category = "SlimeObjectifManager")
	TArray<ASlimeObjectif*> GetObjectifs();

	// Event dispatched when all objectives are dead
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAllObjectivesDeadDelegate);
    
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAllObjectivesDeadDelegate OnAllObjectivesDead;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FindObjectifs();
	
	TArray<ASlimeObjectif*> Objectifs;

	// Called when an objectif dies
	UFUNCTION()
	void OnObjectifDeath(ASlimeObjectif* DeadObjectif);
    
	// Check if all objectives are dead
	void CheckAllObjectivesDead();
    
	// Count of objectives that are still alive
	int32 AliveObjectifsCount;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

};
