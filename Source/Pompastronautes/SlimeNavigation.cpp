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

#include "SlimeNavigation.h"

DEFINE_LOG_CATEGORY(SlimeNAV_LOG);

// Sets default values
ASlimeNavigation::ASlimeNavigation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAutoLoadGrid = true;
	DebugLinesThickness = 0.0f;
}

// Called when the game starts or when spawned
void ASlimeNavigation::BeginPlay()
{
	Super::BeginPlay();
	if (bAutoLoadGrid) {
		LoadGrid();
	}
}

// Called every frame
void ASlimeNavigation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASlimeNavigation::AddGridNode(int32 SavedIndex, FVector Location, FVector Normal)
{
	FSlimeNavNode NavNode;
	NavNode.Location = Location;
	NavNode.Normal = Normal;

	int32 Index = NavNodes.Add(NavNode);
	NavNodes[Index].Index = Index;

	NodesSavedIndexes.Add(SavedIndex, Index);

	FIntVector CellCoord(
	FMath::FloorToInt(Location.X / SpatialHashCellSize),
	FMath::FloorToInt(Location.Y / SpatialHashCellSize),
	FMath::FloorToInt(Location.Z / SpatialHashCellSize)
	);
	SpatialHash.FindOrAdd(CellCoord).Add(Index);	
}

void ASlimeNavigation::SetGridNodeNeighbors(int32 SavedIndex, TArray<int32> NeighborsSavedIndexes)
{
	int32* Index = NodesSavedIndexes.Find(SavedIndex);
	if (Index) {
		FSlimeNavNode* NavNode =  &(NavNodes[*Index]);
		for (int32 i = 0; i != NeighborsSavedIndexes.Num(); ++i) {
			 int32* NeighborIndex = NodesSavedIndexes.Find(NeighborsSavedIndexes[i]);
			 if (NeighborIndex) {
				 FSlimeNavNode* NeighborNode = &(NavNodes[*NeighborIndex]);
				 NavNode->Neighbors.Add(NeighborNode);
			 }
		}
	}
}

int32 ASlimeNavigation::GetNavNodesCount()
{
	return NavNodes.Num();
}

TArray<FVector> ASlimeNavigation::FindPath(FVector Start, FVector End, bool& bFoundCompletePath, float pathJitter)
{
	TArray<FVector> Path;

	FSlimeNavNode* StartNode = FindClosestNode(Start);
	FSlimeNavNode* EndNode = FindClosestNode(End);
	TArray<FSlimeNavNode*> NodesPath = FindNodesPath(StartNode, EndNode, bFoundCompletePath, pathJitter);

	if (SimplifyPaths) {
		TrySimplifyPath(NodesPath);
	}

	for (int32 i = 0; i < NodesPath.Num(); i++) {
		FSlimeNavNode* Node = NodesPath[i];
		Path.Add(Node->Location);
	}

	return Path;
}

TArray<FSlimeNavNode*> ASlimeNavigation::FindNodesPath(FSlimeNavNode* StartNode, FSlimeNavNode* EndNode, bool& bFoundCompletePath, float pathJitter)
{
    TArray<FSlimeNavNode*> Path;
    bFoundCompletePath = false;

    if (!StartNode || !EndNode)
    {
        UE_LOG(SlimeNAV_LOG, Warning, TEXT("Invalid start/end nodes"));
        return Path;
    }

    // Increment generation for incremental metric reset
    CurrentGeneration++;
    
    // Priority queue using F-value comparison
    std::priority_queue<FSlimeNavNode*, std::vector<FSlimeNavNode*>, LessThanByNodeF> OpenQueue;

    // Initialize start node
    StartNode->ResetMetrics();
    StartNode->Generation = CurrentGeneration;
    StartNode->Opened = true;
    OpenQueue.push(StartNode);

    TArray<FSlimeNavNode*> ClosedList;

    while (!OpenQueue.empty())
    {
        FSlimeNavNode* CurrentNode = OpenQueue.top();
        OpenQueue.pop();

        // Skip already processed nodes
        if (CurrentNode->Closed) continue;
        
        CurrentNode->Closed = true;
        ClosedList.Add(CurrentNode);

        // Early exit if target found
        if (CurrentNode == EndNode)
        {
            bFoundCompletePath = true;
            return BuildNodesPathFromEndNode(CurrentNode);
        }

        // Process neighbors
        for (FSlimeNavNode* Neighbor : CurrentNode->Neighbors)
        {
            // Reset neighbor metrics if from previous generation
            if (Neighbor->Generation != CurrentGeneration)
            {
                Neighbor->ResetMetrics();
                Neighbor->Generation = CurrentGeneration;
            }

            if (Neighbor->Closed) continue;

            const float TentativeG = CurrentNode->G + (Neighbor->Location - CurrentNode->Location).Size();

            // Update neighbor if better path found
            if (!Neighbor->Opened || TentativeG < Neighbor->G)
            {
                Neighbor->ParentIndex = CurrentNode->Index;
                Neighbor->G = TentativeG;

            	
                Neighbor->H = (Neighbor->Location - EndNode->Location).Size();

            	if (pathJitter > 0.0f)
				{
					float RandomFactor = FMath::FRandRange(-pathJitter, pathJitter);
					Neighbor->H *= (1.0f + RandomFactor);
				}
				            	
                Neighbor->F = Neighbor->G + Neighbor->H;

                if (!Neighbor->Opened)
                {
                    Neighbor->Opened = true;
                    OpenQueue.push(Neighbor);
                }
                else
                {
                    // Push new entry with better priority
                    OpenQueue.push(Neighbor);
                }
            }
        }
    }

    // Fallback: Find closest node to target in closed list
    if (!bFoundCompletePath && ClosedList.Num() > 0)
    {
        FSlimeNavNode* BestNode = nullptr;
        float MinH = FLT_MAX;

        for (FSlimeNavNode* Node : ClosedList)
        {
            if (Node->H < MinH && Node->ParentIndex > -1)
            {
                MinH = Node->H;
                BestNode = Node;
            }
        }

        if (BestNode)
        {
            UE_LOG(SlimeNAV_LOG, Warning, TEXT("Returning best approximation (H=%.2f)"), MinH);
            return BuildNodesPathFromEndNode(BestNode);
        }
    }
    else
    {
	    UE_LOG(SlimeNAV_LOG, Warning, TEXT("No path found"));
    }

    return Path;
}

FSlimeNavNode* ASlimeNavigation::GetFromOpenList()
{
	float MinF = 9999999999.0f;
	FSlimeNavNode* MinNode = NULL;
	for (FSlimeNavNode* Node : OpenList) {
		if (Node->F < MinF) {
			MinF = Node->F;
			MinNode = Node;
		}
	}
	if (MinNode) {
		OpenList.Remove(MinNode);
	}

	return MinNode;
}

// Updated FindClosestNode
FSlimeNavNode* ASlimeNavigation::FindClosestNode(FVector Location) {
    FSlimeNavNode* ClosestNode = nullptr;
    float MinDistanceSq = FLT_MAX;

    FIntVector TargetCell(
        FMath::FloorToInt(Location.X / SpatialHashCellSize),
        FMath::FloorToInt(Location.Y / SpatialHashCellSize),
        FMath::FloorToInt(Location.Z / SpatialHashCellSize)
    );

    for (int32 X = -1; X <= 1; ++X) {
        for (int32 Y = -1; Y <= 1; ++Y) {
            for (int32 Z = -1; Z <= 1; ++Z) {
                FIntVector CurrentCell = TargetCell + FIntVector(X, Y, Z);
                if (TArray<int32>* NodeIndices = SpatialHash.Find(CurrentCell)) {
                    for (int32 Index : *NodeIndices) {
                        FSlimeNavNode& Node = NavNodes[Index];
                        float DistanceSq = FVector::DistSquared(Node.Location, Location);
                        if (DistanceSq < MinDistanceSq) {
                            MinDistanceSq = DistanceSq;
                            ClosestNode = &Node;
                        }
                    }
                }
            }
        }
    }
    return ClosestNode;
}

void ASlimeNavigation::ResetGridMetrics() {
	CurrentGeneration++;
}

TArray<FVector> ASlimeNavigation::BuildPathFromEndNode(FSlimeNavNode* EndNode)
{
	TArray<FVector> Path;
	TArray<int32> ReversedPathIndexes;

	ReversedPathIndexes.Add(EndNode->Index);

	FSlimeNavNode* IterNode = EndNode;
	while (IterNode->ParentIndex > -1) {
		ReversedPathIndexes.Add(IterNode->ParentIndex);
		IterNode = &NavNodes[IterNode->ParentIndex];
	}

	for (int32 i = ReversedPathIndexes.Num() - 1; i > -1; i--) {
		int32 Index = ReversedPathIndexes[i];
		Path.Add(NavNodes[Index].Location);
	}

	return Path;
}

TArray<FSlimeNavNode*> ASlimeNavigation::BuildNodesPathFromEndNode(FSlimeNavNode* EndNode)
{
	TArray<FSlimeNavNode*> Path;
	TArray<int32> ReversedPathIndexes;

	ReversedPathIndexes.Add(EndNode->Index);

	FSlimeNavNode* IterNode = EndNode;	
	while (IterNode->ParentIndex > -1) {
		ReversedPathIndexes.Add(IterNode->ParentIndex);
		IterNode = &NavNodes[IterNode->ParentIndex];
	}

	for (int32 i = ReversedPathIndexes.Num() - 1; i > -1; i--) {
		int32 Index = ReversedPathIndexes[i];
		Path.Add(&NavNodes[Index]);
	}
	
	return Path;
}

void ASlimeNavigation::TrySimplifyPath(TArray<FSlimeNavNode*>& Path)
{
    TArray<FSlimeNavNode*> NewPath;
    int32 i = 0;
    const float NormalTolerance = 0.01f; // Tolerance for comparing normals

    while (i < Path.Num())
    {
        int32 GroupStart = i;
        FVector GroupNormal = Path[GroupStart]->Normal;

        // Determine the end of the current group of nodes with the same normal
        int32 GroupEnd = i;
        while (GroupEnd < Path.Num() && Path[GroupEnd]->Normal.Equals(GroupNormal, NormalTolerance))
        {
            GroupEnd++;
        }
        GroupEnd--; // Adjust to the last index of the group

        if (GroupEnd > GroupStart)
        {
            FSlimeNavNode* FirstNode = Path[GroupStart];
            FSlimeNavNode* LastNode = Path[GroupEnd];

            // Calculate direction from the first to the last node in the group
            FVector Direction = (LastNode->Location - FirstNode->Location).GetSafeNormal();
            if (Direction.IsNearlyZero()) 
            {
                // If direction is zero, nodes are coincident; just add all and move on
                for (int32 j = GroupStart; j <= GroupEnd; j++)
                {
                    NewPath.Add(Path[j]);
                }
                i = GroupEnd + 1;
                continue;
            }

            // Calculate the cross direction perpendicular to the normal and direction
            FVector CrossDirection = FVector::CrossProduct(GroupNormal, Direction);
            CrossDirection.Normalize();
            CrossDirection *= SlimeRadius;

            // Calculate start and end points for the two traces
            FVector TraceStart1 = FirstNode->Location + (GroupNormal * SlimeRadius) - CrossDirection;
            FVector TraceStart2 = TraceStart1 + 2 * CrossDirection;
            FVector TraceEnd1 = LastNode->Location + (GroupNormal * SlimeRadius) - CrossDirection;
            FVector TraceEnd2 = TraceEnd1 + 2 * CrossDirection;


            // Perform line traces
            bool bHit1 = GetWorld()->LineTraceTestByChannel( TraceStart1, TraceEnd1, ECC_GameTraceChannel1);
            bool bHit2 = GetWorld()->LineTraceTestByChannel(TraceStart2, TraceEnd2, ECC_GameTraceChannel1);
        	
        
            if (!bHit1 && !bHit2)
            {
                // Both traces are clear; add only the first and last nodes of the group
                NewPath.Add(FirstNode);
                NewPath.Add(LastNode);
            }
            else
            {
                // Traces hit something; retain all nodes in the group
                for (int32 j = GroupStart; j <= GroupEnd; j++)
                {
                    NewPath.Add(Path[j]);
                }
            }
        }
        else
        {
            // Single node in the group; add it directly
            NewPath.Add(Path[GroupStart]);
        }

        i = GroupEnd + 1; // Move to the next group
    }

    // Replace the original path with the simplified version
    Path = NewPath;
}

void ASlimeNavigation::TrySimplifyPathExpensive(TArray<FSlimeNavNode*>& Path)
{
    /*// If path is too short or empty, no real simplification needed
    if (Path.Num() < 3)
    {
        return;
    }

    TArray<FSlimeNavNode*> SimplifiedPath;
    SimplifiedPath.Reserve(Path.Num());
    
    // Always keep the starting node
    int32 AnchorIndex = 0;
    SimplifiedPath.Add(Path[AnchorIndex]);

    // Lambda for collision check from node A to node B
    auto CanGoDirect = [&](FSlimeNavNode* FromNode, FSlimeNavNode* ToNode) -> bool
    {
        // Example: a single capsule or sphere trace from FromNode->Location to ToNode->Location
        // Adjust if you prefer line-trace plus offsets for SlimeRadius, etc.
        FCollisionShape SphereShape = FCollisionShape::MakeSphere(SlimeRadius);

        FCollisionQueryParams QueryParams;
        QueryParams.bTraceComplex = false;  // Adjust as needed
        QueryParams.AddIgnoredActor(this);  // Ignore self, etc.

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            /* Start         = #1# FromNode->Location,
            /* End           = #1# ToNode->Location,
            /* Channel       = #1# ECC_GameTraceChannel1,
            /* CollisionShape= #1# SphereShape,
            /* Params        = #1# QueryParams
        );

        // If we HIT something, that means collision is blocking
        return !bHit;
    };

    // While we haven't reached the last node...
    while (AnchorIndex < Path.Num() - 1)
    {
        // We want to find the farthest node we can reach from 'AnchorIndex'
        int32 Low = AnchorIndex + 1;
        int32 High = Path.Num() - 1;
        int32 BestIndex = AnchorIndex; // Start with none

        // Binary search
        while (Low <= High)
        {
            int32 Mid = (Low + High) / 2;

            if (CanGoDirect(Path[AnchorIndex], Path[Mid]))
            {
                // If there's no collision up to Mid, record it
                BestIndex = Mid;
                Low = Mid + 1; // Try going even farther
            }
            else
            {
                High = Mid - 1; // We hit collision, so come back
            }
        }

        // If we cannot move at all (BestIndex == AnchorIndex),
        // it means even going to AnchorIndex+1 is blocked. 
        // This is a fallback to avoid infinite loop in degenerate cases.
        if (BestIndex == AnchorIndex && AnchorIndex + 1 < Path.Num())
        {
            BestIndex = AnchorIndex + 1;
        }

        // We found the farthest direct node from AnchorIndex
        AnchorIndex = BestIndex;
        SimplifiedPath.Add(Path[BestIndex]);

        // If BestIndex is the final node, we are done
        if (BestIndex == Path.Num() - 1)
        {
            break;
        }
    }

    // Replace the original path
    Path = SimplifiedPath;*/
}

bool ASlimeNavigation::LoadGrid()
{
	UE_LOG(SlimeNAV_LOG, Log, TEXT("Start loading Slime nav data"));
	EmptyGrid();
	UE_LOG(SlimeNAV_LOG, Log, TEXT("After empty grid"));

	FVector* NormalRef = NULL;
	FVector Normal;

	FString NameOfSavedLevel = LevelNameToGetGrid;
	if (NameOfSavedLevel.IsEmpty()) {
		NameOfSavedLevel = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
	}
	
	
	USlimeNavGridSaveGame* LoadGameInstance = Cast<USlimeNavGridSaveGame>(UGameplayStatics::CreateSaveGameObject(USlimeNavGridSaveGame::StaticClass()));
	LoadGameInstance->SaveSlotName = TEXT("SlimeNavGrid") + NameOfSavedLevel;
	LoadGameInstance = Cast<USlimeNavGridSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));
	if (LoadGameInstance) {
		UE_LOG(SlimeNAV_LOG, Log, TEXT("After getting load game instance"));
		for (auto It = LoadGameInstance->NavLocations.CreateConstIterator(); It; ++It) {
			NormalRef = LoadGameInstance->NavNormals.Find(It.Key());
			if (NormalRef) {
				Normal = *NormalRef;
			} else {
				Normal = FVector(0.0f, 0.0f, 1.0f);
			}
			AddGridNode(It.Key(), It.Value(), Normal);
		}
		UE_LOG(SlimeNAV_LOG, Log, TEXT("After setting locations"));

		for (auto It = LoadGameInstance->NavRelations.CreateConstIterator(); It; ++It) {
			SetGridNodeNeighbors(It.Key(), It.Value().Neighbors);
		}
		UE_LOG(SlimeNAV_LOG, Log, TEXT("After setting relations"));

		UE_LOG(SlimeNAV_LOG, Log, TEXT("Nav Nodes Loaded: %d"), GetNavNodesCount());

		return true;
	}
	UE_LOG(SlimeNAV_LOG, Warning, TEXT("Not found load game instance"));
	return false;
}

void ASlimeNavigation::EmptyGrid()
{
	NodesSavedIndexes.Empty();
	NavNodes.Empty();
}


void ASlimeNavigation::DrawDebugRelations()
{
	FColor DrawColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f).ToFColor(true);
	FColor DrawColorNormal = FLinearColor(0.6f, 0.3f, 0.3f, 1.0f).ToFColor(true);
	float DrawDuration = 20.0f;
	bool DrawShadow = false;

	for (int32 i = 0; i != NavNodes.Num(); ++i) {
		FSlimeNavNode Nav = NavNodes[i];

		
		//DrawDebugString(GetWorld(), Nav.Location, *FString::Printf(TEXT("[%d]"), Nav.Neighbors.Num()), NULL, DrawColor, DrawDuration, DrawShadow);


		for (int32 j = 0; j != Nav.Neighbors.Num(); ++j) {
			FSlimeNavNode* NeighborNav = Nav.Neighbors[j];
			DrawDebugLine(
				GetWorld(),
				Nav.Location,
				NeighborNav->Location,
				DrawColor,
				false,
				DrawDuration,
				0,
				DebugLinesThickness
			);
		}

		DrawDebugLine(
			GetWorld(),
			Nav.Location,
			Nav.Location + Nav.Normal * 20.0f,
			DrawColorNormal,
			false,
			DrawDuration,
			0,
			DebugLinesThickness*5
		);


	}
}

FVector ASlimeNavigation::FindClosestNodeLocation(FVector Location)
{
	FVector NodeLocation;
	FSlimeNavNode* Node = FindClosestNode(Location);
	if (Node) {
		NodeLocation = Node->Location;
	}
	return NodeLocation;
}

FVector ASlimeNavigation::FindClosestNodeNormal(FVector Location)
{
	FVector NodeNormal;
	FSlimeNavNode* Node = FindClosestNode(Location);
	if (Node) {
		NodeNormal = Node->Normal;
	}
	return NodeNormal;
}

FSlimeNavNode ASlimeNavigation::GetClosestNode(FVector Location)
{
	FSlimeNavNode Node;
	FSlimeNavNode* ClosestNode = FindClosestNode(Location);
	if (ClosestNode) {
		Node = *ClosestNode;
	}
	return Node;
}

bool ASlimeNavigation::FindNextLocationAndNormal(FVector CurrentLocation, FVector TargetLocation, FVector& NextLocation, FVector& Normal)
{
	FSlimeNavNode* StartNode = FindClosestNode(CurrentLocation);
	FSlimeNavNode* EndNode = FindClosestNode(TargetLocation);
	bool bFoundPartialPath;

	FSlimeNavNode* NextNode = NULL;

	TArray<FSlimeNavNode*> NodesPath = FindNodesPath(StartNode, EndNode, bFoundPartialPath);
	
	if (NodesPath.Num() > 1) {
		NextNode = NodesPath[1];
	}

	if (!NextNode) {
		return false;
	}

	NextLocation = NextNode->Location;
	Normal = NextNode->Normal;

	return true;
}
