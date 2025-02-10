// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeObjectif.h"

// Sets default values
ASlimeObjectif::ASlimeObjectif()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

FVector ASlimeObjectif::GetNextAttackPoint()
{
	if (AttackPoints.Num() == 0) {
		FindAttackPoints();
	}

	if (AttackPoints.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("No attack points found"));
		return FVector::ZeroVector;
	}
	
	FVector AttackPoint = AttackPoints[CurrentAttackPointIndex];
	CurrentAttackPointIndex = (CurrentAttackPointIndex + 1) % AttackPoints.Num();
	return AttackPoint;
}

// Called when the game starts or when spawned
void ASlimeObjectif::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASlimeObjectif::FindAttackPoints()
{
	TArray<USceneComponent*> SceneComponents;
	GetComponents(USceneComponent::StaticClass(), SceneComponents);	
	for (int32 i = 0; i < SceneComponents.Num(); ++i) {
		AttackPoints.Add(SceneComponents[i]->GetComponentLocation());
		DrawDebugSphere(GetWorld(), SceneComponents[i]->GetComponentLocation(), 10.0f, 12, FColor::Red, true);
	}
	UE_LOG(LogTemp, Warning, TEXT("AttackPoints: %d"), AttackPoints.Num());

	if (AttackPoints.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("No attack points found"));
	}	
}

// Called every frame
void ASlimeObjectif::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

