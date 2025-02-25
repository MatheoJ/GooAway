// Fill out your copyright notice in the Description page of Project Settings.


#include "BluePrintNodeUtils.h"

AActor* UBluePrintNodeUtils::GetClosestActor(TArray<AActor*> Actors, FVector Location)
{
	AActor* ClosestActor = nullptr;
	float MinDistance = 1000000000.0f;

	for (int32 i = 0; i < Actors.Num(); ++i) {
		AActor* Actor = Actors[i];
		float Distance = FVector::Dist(Location, Actor->GetActorLocation());
		if (Distance < MinDistance) {
			MinDistance = Distance;
			ClosestActor = Actor;
		}
	}

	return ClosestActor;
}
