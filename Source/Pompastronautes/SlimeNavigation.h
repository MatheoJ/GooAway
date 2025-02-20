//The MIT License
//
//Copyright(C) 2017 Roman Nix
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

#pragma once

#include <algorithm>
#include <queue>
#include <vector>         // std::vector
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "SlimeNavGridSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "SlimeNavigation.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(SlimeNAV_LOG, Log, All);

/** Describes navigation point in grid */
USTRUCT(BlueprintType)
struct FSlimeNavNode
{
	GENERATED_BODY()

    /** Location of node */
	UPROPERTY(BlueprintReadOnly)
	FVector Location;

    /** Normal of node from nearest world object with collision */
	UPROPERTY(BlueprintReadOnly)
	FVector Normal;

    /** Index (id) of node */
	UPROPERTY()
	int32 Index;

    /** Relations */
	TArray <FSlimeNavNode*> Neighbors;

    /** F-value of node from A-star */
	float F;

	/** G-value of node from A-star */
	float G;

	/** H-value of node from A-star */
	float H;

	/** Opened property of node from A-star */
	bool Opened;

	/** Closed propery of node from A-star */
	bool Closed;

	/** Index (id) of parent node from A-star */
	int32 ParentIndex;

	// Count the generation of the use of the node for pathfinding
	uint32 Generation =0;

    /** Initialization of node */
	FSlimeNavNode()
	{
		Location = FVector(0.0f, 0.0f, 0.0f);
		Index = -1;
		F = 0.0f;
		G = 0.0f;
		H = 0.0f;
		Opened = false;
		Closed = false;
		ParentIndex = -1;

		Neighbors.Empty();
	}

    /** Resets metriks of A-star */
	void ResetMetrics()
	{
		F = 0.0f;
		G = 0.0f;
		H = 0.0f;
		Opened = false;
		Closed = false;
		ParentIndex = -1;
	}
};

/** Comparator < of nodes by F-value */
struct LessThanByNodeF {
	bool operator()(const FSlimeNavNode* lhs, const FSlimeNavNode* rhs) const {
		return lhs->F > rhs->F;
	}
};

/**  Comparator < of nodes by H-value */
struct LessThanByNodeH {
	bool operator()(const FSlimeNavNode* lhs, const FSlimeNavNode* rhs) const {
		return lhs->H > rhs->H;
	}
};

/** Class for navigation between nodes with A-star */
UCLASS()
class ASlimeNavigation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlimeNavigation();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FSlimeNavNode> NavNodes;
	// SavedIndex -> LocalIndex
	TMap<int32, int32> NodesSavedIndexes;
	
	// Add to FSlimeNavNode struct
	uint32 Generation;	

	void ResetGridMetrics();
	TArray<FVector> BuildPathFromEndNode(FSlimeNavNode* EndNode);

    FSlimeNavNode* FindClosestNode(FVector Location);

	void AddGridNode(int32 SavedIndex, FVector Location, FVector Normal);
	void SetGridNodeNeighbors(int32 SavedIndex, TArray<int32> NeighborsSavedIndexes);
	void EmptyGrid();

	TArray<FSlimeNavNode*> FindNodesPath(FSlimeNavNode* StartNode, FSlimeNavNode* EndNode, bool& bFoundCompletePath, float pathJitter= -1.0f);
	TArray<FSlimeNavNode*> BuildNodesPathFromEndNode(FSlimeNavNode* EndNode);

	void TrySimplifyPath(TArray<FSlimeNavNode*> & Path);

	void TrySimplifyPathExpensive(TArray<FSlimeNavNode*> & Path);

	TArray<FSlimeNavNode*> OpenList;
	FSlimeNavNode* GetFromOpenList();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavigation")
	float SpatialHashCellSize = 100.0f;

	TMap<FIntVector, TArray<int32>> SpatialHash;
	uint32 CurrentGeneration =0;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Whether to load the navigation grid on BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavigation")
	bool bAutoLoadGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavigation")
	float SlimeRadius = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavigation")
	bool SimplifyPaths = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavigation")
	FString LevelNameToGetGrid;
	
    /** Thickness of debug lines */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	float DebugLinesThickness = 2.0f;

	/** Returns number of navigation nodes */
	UFUNCTION(BlueprintCallable, Category = "SlimeNavigation")
	int32 GetNavNodesCount();

	/** Finds path in grid. Returns array of nodes */
	UFUNCTION(BlueprintCallable, Category = "SlimeNavigation")
	TArray<FVector> FindPath(FVector Start, FVector End, bool& bFoundCompletePath, float pathJitter = 0.0f);

    /** Loads navigation grid from file */
	UFUNCTION(BlueprintCallable, Category = "SlimeNavigation")
    bool LoadGrid();

    /** Draws debug lines between connected nodes */
	UFUNCTION(BlueprintCallable, Category = "SlimeNavigation")
	void DrawDebugRelations();

    /** Finds closest node's location in grid to specified location */
	UFUNCTION(BlueprintCallable, Category = "SlimeNavigation")
	FVector FindClosestNodeLocation(FVector Location);

    /** Finds closest node's normal in grid to specified location  */
	UFUNCTION(BlueprintCallable, Category = "SlimeNavigation")
	FVector FindClosestNodeNormal(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "SlimeNavigation")
	FSlimeNavNode GetClosestNode(FVector Location);

    /** Finds path between current location and target location and returns location and normal of the next fisrt node in navigation grid */
	UFUNCTION(BlueprintCallable, Category = "SlimeNavigation")
	bool FindNextLocationAndNormal(FVector CurrentLocation, FVector TargetLocation, FVector& NextLocation, FVector& Normal);
};
