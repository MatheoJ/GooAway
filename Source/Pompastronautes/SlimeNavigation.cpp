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

TArray<FVector> ASlimeNavigation::FindPath(FVector Start, FVector End, bool& bFoundCompletePath)
{
	TArray<FVector> Path;

	FSlimeNavNode* StartNode = FindClosestNode(Start);
	FSlimeNavNode* EndNode = FindClosestNode(End);
	TArray<FSlimeNavNode*> NodesPath = FindNodesPath(StartNode, EndNode, bFoundCompletePath);

	for (int32 i = 0; i < NodesPath.Num(); i++) {
		FSlimeNavNode* Node = NodesPath[i];
		Path.Add(Node->Location);
	}

	return Path;
}

TArray<FSlimeNavNode*> ASlimeNavigation::FindNodesPath(FSlimeNavNode* StartNode, FSlimeNavNode* EndNode, bool& bFoundCompletePath)
{
	TArray<FSlimeNavNode*> Path;
	FSlimeNavNode* Node = NULL;
	TArray <FSlimeNavNode*> Neighbors;

	if (!StartNode || !EndNode) {
		//GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Yellow, TEXT("Not found closest nodes"));
		UE_LOG(SlimeNAV_LOG, Warning, TEXT("Not found closest nodes"));
		return Path;
	}
	ResetGridMetrics();
	//OpenList.Empty();
	std::vector<FSlimeNavNode*> openList;
	TArray<FSlimeNavNode*> ClosedList;

	//OpenList.Add(StartNode);
	openList.push_back(StartNode);
	std::make_heap(openList.begin(), openList.end(), LessThanByNodeF());
	StartNode->Opened = true;


	while (openList.size()) {
		//Node = GetFromOpenList();
		std::pop_heap(openList.begin(), openList.end(), LessThanByNodeF());
		Node = openList.back();
		openList.pop_back();

		Node->Closed = true;
		if (Node != StartNode) {
			ClosedList.Add(Node);
		}

		if (Node->Index == EndNode->Index) {
			bFoundCompletePath = true;
			return BuildNodesPathFromEndNode(Node);
		}

		for (FSlimeNavNode* Neighbor : Node->Neighbors) {

			if (Neighbor->Closed) {
				continue;
			}

			//DrawDebugString(GetWorld(), Neighbor->Location, *FString::Printf(TEXT("[%d]"), Neighbor->Neighbors.Num()), NULL, FLinearColor(0.0f, 1.0f, 0.0f, 1.0f).ToFColor(true), 20.0f, false);

			// get the distance between current node and the neighbor
			// and calculate the next g score
			float NewG = Node->G + (Neighbor->Location - Node->Location).Size();

			// check if the neighbor has not been inspected yet, or
			// can be reached with smaller cost from the current node
			if (!Neighbor->Opened || NewG < Neighbor->G) {
				Neighbor->G = NewG;
				Neighbor->H = (Neighbor->Location - EndNode->Location).Size();
				Neighbor->F = Neighbor->G + Neighbor->H;
				Neighbor->ParentIndex = Node->Index;

				if (!Neighbor->Opened) {
					//OpenList.Add(Neighbor);
					openList.push_back(Neighbor);
					std::push_heap(openList.begin(), openList.end(), LessThanByNodeF());
					Neighbor->Opened = true;
				} else {
					// the neighbor can be reached with smaller cost.
					// Since its f value has been updated, we have to
					// update its position in the open list
					std::make_heap(openList.begin(), openList.end(), LessThanByNodeF());
				}
			}
		}
	}

	UE_LOG(SlimeNAV_LOG, Warning, TEXT("Not found complete path"));


	//Finding closest to end
	float IterMin = 99999999999.0f;
	for (FSlimeNavNode* IterNode : ClosedList) {
		if (IterNode->F < IterMin) {
			IterMin = IterNode->F;
			Node = IterNode;
		}
		//DrawDebugString(GetWorld(), closedList[i]->Location, *FString::Printf(TEXT("[%f, %f]"), closedList[i]->F, closedList[i]->H), NULL, FLinearColor(0.0f, 1.0f, 0.0f, 1.0f).ToFColor(true), DebugLinesThickness, false);
	}

	if (Node) {
		UE_LOG(SlimeNAV_LOG, Log, TEXT("Min F = %f"), Node->F);
		bFoundCompletePath = false;
		//DrawDebugString(GetWorld(), Node->Location, *FString::Printf(TEXT("MINH[%f]"), Node->H), NULL, FLinearColor(1.0f, 0.0f, 0.0f, 1.0f).ToFColor(true), DebugLinesThickness, false);

		return BuildNodesPathFromEndNode(Node);
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

FSlimeNavNode* ASlimeNavigation::FindClosestNode(FVector Location)
{
	FSlimeNavNode* ClosestNode = nullptr;
	float MinDistance = 999999999.f;
	for (int32 i = 0; i != NavNodes.Num(); i++) {
		float Distance = (NavNodes[i].Location - Location).Size();
		if (Distance < MinDistance) {
			MinDistance = Distance;
			ClosestNode = &(NavNodes[i]);
		}
	}

	return ClosestNode;
}

void ASlimeNavigation::ResetGridMetrics()
{
	for (int32 i = 0; i != NavNodes.Num(); ++i) {
		FSlimeNavNode Node = NavNodes[i];
		Node.ResetMetrics();
		NavNodes[i] = Node;
	}
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

bool ASlimeNavigation::LoadGrid()
{
	UE_LOG(SlimeNAV_LOG, Log, TEXT("Start loading Slime nav data"));
	EmptyGrid();
	UE_LOG(SlimeNAV_LOG, Log, TEXT("After empty grid"));

	FVector* NormalRef = NULL;
	FVector Normal;

	USlimeNavGridSaveGame* LoadGameInstance = Cast<USlimeNavGridSaveGame>(UGameplayStatics::CreateSaveGameObject(USlimeNavGridSaveGame::StaticClass()));
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
	FColor DrawColorNormal = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f).ToFColor(true);
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
			Nav.Location + Nav.Normal * 100.0f,
			DrawColorNormal,
			false,
			DrawDuration,
			0,
			DebugLinesThickness
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
