// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeObjectifManager.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ASlimeObjectifManager::ASlimeObjectifManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

FVector ASlimeObjectifManager::GetAttackPointFromClosestObjectif(AActor* actor)
{
	
	FVector actorLocation = actor->GetActorLocation();
	float minDistance = 1000000000.0f;
	ASlimeObjectif* closestOjectif = nullptr;

	if (Objectifs.Num() == 0) {
		FindObjectifs();
	}

	for (int32 i = 0; i < Objectifs.Num(); ++i) {
		ASlimeObjectif* Objectif = Objectifs[i];
		float distance = FVector::Dist(actorLocation, Objectif->GetActorLocation());
		if (distance < minDistance) {
			minDistance = distance;
			closestOjectif = Objectif;
		}
	}

	if (closestOjectif) {
		return closestOjectif->GetNextAttackPoint();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("No objectif found"));
		return FVector::ZeroVector;
	}
}

ASlimeObjectif* ASlimeObjectifManager::GetClossestObjectif(AActor* actor)
{
	FVector actorLocation = actor->GetActorLocation();
	float minDistance = 1000000000.0f;
	ASlimeObjectif* closestOjectif = nullptr;

	if (Objectifs.Num() == 0) {
		FindObjectifs();
	}

	for (int32 i = 0; i < Objectifs.Num(); ++i) {
		ASlimeObjectif* Objectif = Objectifs[i];
		float distance = FVector::Dist(actorLocation, Objectif->GetActorLocation());
		if (distance < minDistance) {
			minDistance = distance;
			closestOjectif = Objectif;
		}
	}

	if (closestOjectif) {
		return closestOjectif;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("No objectif found"));
		return nullptr;
	}
}

// Called when the game starts or when spawned
void ASlimeObjectifManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASlimeObjectifManager::FindObjectifs()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASlimeObjectif::StaticClass(), FoundActors);
	for (int32 i = 0; i < FoundActors.Num(); ++i) {
		ASlimeObjectif* Objectif = Cast<ASlimeObjectif>(FoundActors[i]);
		Objectifs.Add(Objectif);
	}

	UE_LOG(LogTemp, Warning, TEXT("Objectifs: %d"), Objectifs.Num());
}

// Called every frame
void ASlimeObjectifManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

