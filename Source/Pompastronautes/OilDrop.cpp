// Fill out your copyright notice in the Description page of Project Settings.


#include "OilDrop.h"

// Sets default values
AOilDrop::AOilDrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOilDrop::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOilDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOilDrop::SetOnFire_Implementation()
{
	// Your implementation code here
}

