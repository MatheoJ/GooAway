// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlimeNavPoint.generated.h"

UCLASS()
class POMPASTRONAUTES_API ASlimeNavPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlimeNavPoint();

	/** Sphere collision component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeGridBuilder")
	class USphereComponent* CollisionComp;

	/** Nearest navigation points which can be connected */
	TArray<ASlimeNavPoint*> Neighbors;

	/** Nearest navigation points which can be possible connected */
	TArray<ASlimeNavPoint*> PossibleEdgeNeighbors;

	/** Normal from nearest world object with collision */
	FVector Normal;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
