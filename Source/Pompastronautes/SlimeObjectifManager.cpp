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
		if (!Objectif->bIsDead && distance < minDistance) {
			minDistance = distance;
			closestOjectif = Objectif;
		}
	}

	if (closestOjectif) {
		return closestOjectif;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No objectif found"));
		return nullptr;
	}
}

TArray<ASlimeObjectif*> ASlimeObjectifManager::GetObjectifs()
{
	if (Objectifs.Num() == 0) {
		FindObjectifs();
	}
	return Objectifs;
}

void ASlimeObjectifManager::StopAllObjectifsDamage()
{
	for (int32 i = 0; i < Objectifs.Num(); ++i) {
		ASlimeObjectif* Objectif = Objectifs[i];
		Objectif->canTakeDamage = false;
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

	Objectifs.Empty();
	AliveObjectifsCount = 0;
	for (int32 i = 0; i < FoundActors.Num(); ++i) {
		ASlimeObjectif* Objectif = Cast<ASlimeObjectif>(FoundActors[i]);
		if (Objectif)
		{
			Objectifs.Add(Objectif);
            
			// Only count and bind to non-dead objectives
			if (!Objectif->bIsDead)
			{
				AliveObjectifsCount++;
				// Bind to the death event
				Objectif->OnObjectifDeath.AddDynamic(this, &ASlimeObjectifManager::OnObjectifDeath);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Objectifs: %d, Alive: %d"), Objectifs.Num(), AliveObjectifsCount);
}

void ASlimeObjectifManager::OnObjectifDeath(ASlimeObjectif* DeadObjectif)
{
	// Decrement our alive count
	AliveObjectifsCount--;
	UE_LOG(LogTemp, Warning, TEXT("Objectif died! Remaining: %d"), AliveObjectifsCount);
    
	// Check if all objectives are now dead
	CheckAllObjectivesDead();
}

void ASlimeObjectifManager::CheckAllObjectivesDead()
{
	if (AliveObjectifsCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("All objectives are dead!"));
		// Broadcast the event
		OnAllObjectivesDead.Broadcast();
	}
}

// Called every frame
void ASlimeObjectifManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

