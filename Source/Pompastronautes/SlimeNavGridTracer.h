// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlimeNavGridTracer.generated.h"

UCLASS()
class POMPASTRONAUTES_API ASlimeNavGridTracer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlimeNavGridTracer();

	/** Sphere collision component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	class USphereComponent* CollisionComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
