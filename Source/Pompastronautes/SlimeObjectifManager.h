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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FindObjectifs();
	
	TArray<ASlimeObjectif*> Objectifs;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

};
