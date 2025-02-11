// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dummyslime2.generated.h"

UCLASS()
class POMPASTRONAUTES_API ADummyslime2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADummyslime2();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cpp")
	float oui=5.f;

	UFUNCTION(BlueprintCallable, Category = "Cpp")
	void Sucking();

	bool isGettingSucked;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
