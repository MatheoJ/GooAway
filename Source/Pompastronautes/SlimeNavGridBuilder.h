// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "SlimeNavGridTracer.h"
#include "SlimeNavPoint.h"
#include "SlimeNavPointEdge.h"
#include "SlimeNavGridSaveGame.h"
#include "SlimeNavGridBuilder.generated.h"

UENUM(BlueprintType)
enum class EGridBuildStep : uint8
{
	None,
	EmptyAll,
	SpawnTracers,
	RemoveTracersEnclosed,
	TraceFromTracers,
	RemoveAllTracers,
	SpawnNavPoints,
	BuildRelations,
	BuildEdgeRelations,
	RemoveNoConnected,
	SaveGrid,
	Done
};



DECLARE_LOG_CATEGORY_EXTERN(SlimeNAVGRID_LOG, Log, All);
UCLASS()
class POMPASTRONAUTES_API ASlimeNavGridBuilder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlimeNavGridBuilder();

	/** Volume where to build navigation */
	UPROPERTY(VisibleAnywhere, Category="SlimeNavGridBuilder")
		UBoxComponent* VolumeBox;

	/** For debug. Blueprint class which will be used to spawn actors on scene in specified volume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
		TSubclassOf<ASlimeNavGridTracer> TracerActorBP;

	/** For debug. Blueprint class which will be used to spawn Navigation Points */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
		TSubclassOf<ASlimeNavPoint> NavPointActorBP;

	/** For debug. Blueprint class which will be used to spawn Navigation Points on egdes when checking possible neightbors */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
		TSubclassOf<ASlimeNavPointEdge> NavPointEdgeActorBP;

	/** The minimum distance between tracers to fill up scene */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	float GridStepSize;

	/** The list of actors which could have navigation points on them */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	TArray<AActor*> ActorsWhiteList;

	/** Whether to use ActorsWhiteList */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	bool bUseActorWhiteList;

	/** The list of actors which COULD NOT have navigation points on them */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	TArray<AActor*> ActorsBlackList;

	/** Whether to use ActorsBlackList */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	bool bUseActorBlackList;

	/** For debug. If false then all tracers remain on the scene after grid rebuild */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	bool bAutoRemoveTracers;

	/** Whether to save the navigation grid after rebuild */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	bool bAutoSaveGrid;

	/** How far put navigation point from a WorldStatic face */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	float BounceNavDistance;

	/** How far to trace from tracers. Multiplier of GridStepSize */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	float TraceDistanceModificator;

	/** How close navigation points can be to each other. Multiplier of GridStepSize */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	float ClosePointsFilterModificator;

	/** The radius of a sphere to find neighbors of each NavPoint. Multiplier of GridStepSize */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	float ConnectionSphereRadiusModificator;

	/** How far to trace from each NavPoint to find intersection through egdes of possible neightbors. Multiplier of GridStepSize */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	float TraceDistanceForEdgesModificator;

	/** How far can be one trace line from other trace line near the point of intersection when checking possible neightbors. Multiplier of GridStepSize */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
	float EgdeDeviationModificator;

    /** Whether should try to remove tracers enclosed in volumes */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
    bool bShouldTryToRemoveTracersEnclosedInVolumes;

    /** Distance threshold to remove tracers enclosed in volumes  */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeNavGridBuilder")
    float TracersInVolumesCheckDistance;
	
	// Add these properties to your class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime Nav Grid")
	bool bSaveToContentFolder = true;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime Nav Grid")
	FString ContentFolderPath = TEXT("NavData");
    
	// Add these methods
	bool CopySaveGameToContent(const FString& SaveSlotName);
	bool CopySaveGameFromContent(const FString& SaveSlotName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<ASlimeNavGridTracer*> Tracers;
	TArray<ASlimeNavPoint*> NavPoints;

	TArray<FVector> NavPointsLocations;
	TMap<int32, FVector> NavPointsNormals;

	bool TickBuildSteps(float DeltaTime);
	
	void TraceFromAllTracers();

	void RemoveTracersClosedInVolumes();

	void AddNavPointByHitResult(FHitResult RV_Hit);

	void SpawnNavPoints();

	void BuildRelations();

	bool CheckNavPointsVisibility(ASlimeNavPoint* NavPoint1, ASlimeNavPoint* NavPoint2);

	bool CheckNavPointCanSeeLocation(ASlimeNavPoint* NavPoint, FVector Location);

	void BuildPossibleEdgeRelations();

	void RemoveNoConnected();

	void RemoveAllNavPoints();

	void EmptyAll();

	bool GetLineLineIntersection(FVector Start0, FVector End0, FVector Start1, FVector End1, FVector& Intersection);

	void CheckAndAddIntersectionNavPointEdge(FVector Intersection, ASlimeNavPoint* NavPoint1, ASlimeNavPoint* NavPoint2);

	float Dmnop(const TMap<int32, FVector> *v, int32 m, int32 n, int32 o, int32 p);

	void RemoveAllTracers();

	void SpawnTracers();

	int32 GetNavPointIndex(ASlimeNavPoint* NavPoint);

	float DebugThickness;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
    /** Builds navigation grid. Returs number of navigations points */
	UFUNCTION(BlueprintCallable, Category = "SlimeNavGridBuilder")
	int32 BuildGrid();

    /** Draws debug lines between connected navigation points */
	UFUNCTION(BlueprintCallable, Category = "SlimeNavGridBuilder")
	void DrawDebugRelations();

    /** Saves navigation grid to save file */
	UFUNCTION(BlueprintCallable, Category = "SlimeNavGridBuilder")
	void SaveGrid();

private:
	EGridBuildStep CurrentBuildStep = EGridBuildStep::None;
	double BuildStartTime = 0.0;

	
};
