// Fill out your copyright notice in the Description page of Project Settings.


#include "BluePrintNodeUtils.h"

#include "SlimeBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"


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


FName  UBluePrintNodeUtils::GetClosestSocketToLocation(AActor* Actor, FVector TargetLocation, FName SocketsToExclude)
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
		if (Socket && Socket->SocketName != SocketsToExclude)
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

FVector UBluePrintNodeUtils::GetClosestSocketLocation(AActor* Actor, FVector TargetLocation, FName SocketsToExclude)
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
		if (Socket && Socket->SocketName != SocketsToExclude)
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


bool UBluePrintNodeUtils::IsActorInSublevel(AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}

	// Get the actor's level
	ULevel* ActorLevel = Actor->GetLevel();
	if (!ActorLevel)
	{
		return false;
	}

	// Get the world's persistent level
	UWorld* World = Actor->GetWorld();
	if (!World)
	{
		return false;
	}
    
	ULevel* PersistentLevel = World->PersistentLevel;
    
	// If the actor's level is not the persistent level, it's in a sublevel
	return ActorLevel != PersistentLevel;
}

bool UBluePrintNodeUtils::ArePlayersBehindPlane(const TArray<ACharacter*>& Players, const FVector& PlanePosition,
	const FVector& PlaneNormal)
{
	// Return false if no players to check
	if (Players.Num() == 0)
	{
		return false;
	}
    
	// Normalize the plane normal to ensure proper calculations
	FVector NormalizedPlaneNormal = PlaneNormal.GetSafeNormal();
    
	// Check each player
	for (const ACharacter* character : Players)
	{
		if (!character)
		{
			continue;
		}
        
		// Get player position
		FVector PlayerPosition = character->GetActorLocation();
        
		// Calculate vector from plane to player
		FVector PlaneToPlayer = PlayerPosition - PlanePosition;
        
		// Calculate dot product to determine which side of the plane the player is on
		float DotProduct = FVector::DotProduct(PlaneToPlayer, NormalizedPlaneNormal);
        
		// If DotProduct >= 0, player is in front of or on the plane, not behind it
		if (DotProduct >= 0)
		{
			return false;
		}
	}
    
	// If we get here, all checked players are behind the plane
	return true;
}

AOilSpill* UBluePrintNodeUtils::GetClosestOilSpillOnFireInHitArray(const TArray<FHitResult>& HitResults,
	const FVector& ActorLocation, float MaxDistance)
{
	{
    		AOilSpill* ClosestOilSpill = nullptr;
    		float MinDistance = MaxDistance;
    
    		for (const FHitResult& Hit : HitResults)
    		{
    			if (Hit.GetActor())
    			{
    				AOilSpill* OilSpill = Cast<AOilSpill>(Hit.GetActor());
    				if (OilSpill && OilSpill->IsOnFire)
    				{
    					float Distance = FVector::Dist(ActorLocation, Hit.GetActor()->GetActorLocation());
    					if (Distance < MinDistance)
    					{
    						MinDistance = Distance;
    						ClosestOilSpill = OilSpill;
    					}
    				}
    			}
    		}
    
    		return ClosestOilSpill;
	}
}

int UBluePrintNodeUtils::GetIndexOfLastLeaderboardEntry(const TArray<FLeaderboardEntry>& LeaderboardEntries)
{
	FDateTime MaxTimestamp = 0.0f;
	int32 LastIndex = -1;
	for (int32 i = 0; i < LeaderboardEntries.Num(); ++i)
	{
		if (LeaderboardEntries[i].Timestamp > MaxTimestamp)
		{
			MaxTimestamp = LeaderboardEntries[i].Timestamp;
			LastIndex = i;
		}
	}
	return LastIndex;
}

AActor* UBluePrintNodeUtils::GetClosestActorSlimeWithTypeExcept(TArray<AActor*> Actors, FVector Location,
	ESlimeType SlimeTypeToExclude)
{
	AActor* ClosestActor = nullptr;
	float MinDistance = 1000000000.0f;

	for (int32 i = 0; i < Actors.Num(); ++i) {
		AActor* ActorToCheck = Actors[i];

		ASlimeBase* Slime = Cast<ASlimeBase>(ActorToCheck);
		if (Slime && Slime->SlimeType != SlimeTypeToExclude)
		{
			float Distance = FVector::Dist(Location, ActorToCheck->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestActor = ActorToCheck;
			}
		}
	}

	return ClosestActor;
}

bool UBluePrintNodeUtils::ProjectWorldToScreenBidirectional(APlayerController const* Player, const FVector& WorldPosition, FVector2D& ScreenPosition, bool& bTargetBehindCamera, bool bPlayerViewportRelative)
{
	FVector Projected;
	bool bSuccess = false;

	ULocalPlayer* const LP = Player ? Player->GetLocalPlayer() : nullptr;
	if (LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, /*out*/ ProjectionData))
		{
			const FMatrix ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
			const FIntRect ViewRectangle = ProjectionData.GetConstrainedViewRect();

			FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPosition, 1.f));
			if (Result.W < 0.f) { bTargetBehindCamera = true; } else { bTargetBehindCamera = false; }
			if (Result.W == 0.f) { Result.W = 1.f; } // Prevent Divide By Zero

			const float RHW = 1.f / FMath::Abs(Result.W);
			Projected = FVector(Result.X, Result.Y, Result.Z) * RHW;

			// Normalize to 0..1 UI Space
			const float NormX = (Projected.X / 2.f) + 0.5f;
			const float NormY = 1.f - (Projected.Y / 2.f) - 0.5f;

			Projected.X = (float)ViewRectangle.Min.X + (NormX * (float)ViewRectangle.Width());
			Projected.Y = (float)ViewRectangle.Min.Y + (NormY * (float)ViewRectangle.Height());

			bSuccess = true;
			ScreenPosition = FVector2D(Projected.X, Projected.Y);

			if (bPlayerViewportRelative)
			{
				ScreenPosition -= FVector2D(ProjectionData.GetConstrainedViewRect().Min);
			}
		}
		else
		{
			ScreenPosition = FVector2D(1234, 5678);
		}
	}

	return bSuccess;
}

FVector2D UBluePrintNodeUtils::ProjectWidgetPositionToBorder(const FVector2D& WidgetPosition,
	const FVector2D& ViewportSize)
{
	// Calculate distance to each edge
	float DistanceToTop = WidgetPosition.Y;
	float DistanceToBottom = ViewportSize.Y - WidgetPosition.Y;
	float DistanceToLeft = WidgetPosition.X;
	float DistanceToRight = ViewportSize.X - WidgetPosition.X;
    
	// Find the minimum distance
	float MinDistance = FMath::Min<float>(
		DistanceToTop,
		FMath::Min<float>(
			DistanceToBottom,
			FMath::Min<float>(DistanceToLeft, DistanceToRight)
		)
	);
    
	FVector2D ProjectedPosition = WidgetPosition;
    
	// Project to the closest edge
	if (MinDistance == DistanceToTop)
	{
		// Project to top edge
		ProjectedPosition.Y = 0.0f;
	}
	else if (MinDistance == DistanceToBottom)
	{
		// Project to bottom edge
		ProjectedPosition.Y = ViewportSize.Y;
	}
	else if (MinDistance == DistanceToLeft)
	{
		// Project to left edge
		ProjectedPosition.X = 0.0f;
	}
	else // MinDistance == DistanceToRight
	{
		// Project to right edge
		ProjectedPosition.X = ViewportSize.X;
	}
    
	return ProjectedPosition;
}
