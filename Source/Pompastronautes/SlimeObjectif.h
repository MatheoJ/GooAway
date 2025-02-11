// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlimeObjectif.generated.h"

UCLASS()
class POMPASTRONAUTES_API ASlimeObjectif : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlimeObjectif();

	UFUNCTION(BlueprintCallable)
	FVector GetNextAttackPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FindAttackPoints();

	TArray<FVector> AttackPoints;

	int CurrentAttackPointIndex = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
