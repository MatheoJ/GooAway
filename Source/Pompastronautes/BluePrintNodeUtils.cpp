// Fill out your copyright notice in the Description page of Project Settings.


#include "BluePrintNodeUtils.h"
#include "Engine/SkeletalMeshSocket.h"

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


FName  UBluePrintNodeUtils::GetClosestSocketToLocation(AActor* Actor, FVector TargetLocation)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetClosestSocketToLocation: Actor is null"));
		return NAME_None;
	}

	// Get the Skeletal Mesh Component
	USkeletalMeshComponent* SkeletalMeshComp = Actor->FindComponentByClass<USkeletalMeshComponent>();
	if (!SkeletalMeshComp || !SkeletalMeshComp->GetSkinnedAsset())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetClosestSocketToLocation: Skeletal Mesh Component is null or has no mesh"));
		return NAME_None;
	}

	const  USkinnedAsset* SkeletalMesh = SkeletalMeshComp->GetSkinnedAsset();
	const TArray<USkeletalMeshSocket*> Sockets = SkeletalMesh->GetActiveSocketList();

	if (Sockets.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetClosestSocketToLocation: No sockets found"));
		return NAME_None;
	}

	// Find the closest socket
	FName ClosestSocketName = NAME_None;
	float ClosestDistanceSq = FLT_MAX;

	for (const USkeletalMeshSocket* Socket : Sockets)
	{
		if (Socket)
		{
			FVector SocketLocation = SkeletalMeshComp->GetSocketLocation(Socket->SocketName);
			float DistanceSq = FVector::DistSquared(SocketLocation, TargetLocation);

			if (DistanceSq < ClosestDistanceSq)
			{
				ClosestDistanceSq = DistanceSq;
				ClosestSocketName = Socket->SocketName;
			}
		}
	}

	return ClosestSocketName;
}

FVector UBluePrintNodeUtils::GetClosestSocketLocation(AActor* Actor, FVector TargetLocation)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetClosestSocketLocation: Actor is null"));
		return FVector::ZeroVector;
	}

	// Get the Skeletal Mesh Component
	USkeletalMeshComponent* SkeletalMeshComp = Actor->FindComponentByClass<USkeletalMeshComponent>();
	if (!SkeletalMeshComp || !SkeletalMeshComp->GetSkinnedAsset())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetClosestSocketToLocation: Skeletal Mesh Component is null or has no mesh"));
		return FVector::ZeroVector;
	}

	const  USkinnedAsset* SkeletalMesh = SkeletalMeshComp->GetSkinnedAsset();
	const TArray<USkeletalMeshSocket*> Sockets = SkeletalMesh->GetActiveSocketList();

	if (Sockets.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetClosestSocketToLocation: No sockets found"));
		return FVector::ZeroVector;
	}

	// Find the closest socket
	FVector ClosestSocketLocation = FVector::ZeroVector;
	float ClosestDistanceSq = FLT_MAX;

	for (const USkeletalMeshSocket* Socket : Sockets)
	{
		if (Socket)
		{
			FVector SocketLocation = SkeletalMeshComp->GetSocketLocation(Socket->SocketName);
			float DistanceSq = FVector::DistSquared(SocketLocation, TargetLocation);

			if (DistanceSq < ClosestDistanceSq)
			{
				ClosestDistanceSq = DistanceSq;
				ClosestSocketLocation = SocketLocation;
			}
		}
	}

	return ClosestSocketLocation;	
}
