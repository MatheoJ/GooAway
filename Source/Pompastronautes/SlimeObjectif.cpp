// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeObjectif.h"

#include "ObjectifAttackPoint.h"
#include "OilSpill.h"
#include "SlimeBase.h"
#include "Components/ShapeComponent.h"

// Sets default values
ASlimeObjectif::ASlimeObjectif()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CurrentLife = MaxLife;

}

FVector ASlimeObjectif::GetNextAttackPoint()
{
	if (AttackPoints.Num() == 0) {
		FindAttackPoints();
	}

	if (AttackPoints.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("No attack points found on Objectif"));
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

	
	if (!canTakeDamage) {
		return;
	}
	// Find all the collision components
	GetComponents<UShapeComponent>(CollisionComponents);
	//Log the number of components
	UE_LOG(LogTemp, Warning, TEXT("Number of collision components: %d"), CollisionComponents.Num());

	// Start checking every 0.5s
	GetWorldTimerManager().SetTimer(
		TimerHandle_CheckForEnemies,
		this,
		&ASlimeObjectif::UpdateLifeWithAttackingSlime,
		0.5f,
		true
	);
	
}

void ASlimeObjectif::FindAttackPoints()
{
	TArray<UObjectifAttackPoint*> SceneComponents;
	GetComponents(UObjectifAttackPoint::StaticClass(), SceneComponents);
	int numAttackPoints = SceneComponents.Num();
	for (int32 i = 0; i < SceneComponents.Num(); ++i) {
				
		AttackPoints.Add(SceneComponents[i]->GetComponentLocation());
		DrawDebugSphere(GetWorld(), SceneComponents[i]->GetComponentLocation(), 10.0f, 12, FColor::Red, true);
	}
	UE_LOG(LogTemp, Warning, TEXT("AttackPoints: %d"), AttackPoints.Num());

	if (AttackPoints.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("No attack points found"));
	}	
}

void ASlimeObjectif::UpdateLifeWithAttackingSlime()
{
	if (bIsDead || !canTakeDamage) {
		return;
	}
	
	TArray<AActor*> OverlappingActors;
	int NumberOverLappingSlime = 0;
	int NumberFireOnObjectif = 0;

	for (UShapeComponent* PrimComp : CollisionComponents)
	{
		OverlappingActors.Reset();
		// If you have a specific enemy class, use that as the filter		
		PrimComp->GetOverlappingActors(OverlappingActors, ASlimeBase::StaticClass());
		NumberOverLappingSlime += OverlappingActors.Num();

		if (bTakeDamageFromFireAndExplosion)
		{
			//Get all AOilSpill overlapping actors
			TArray<AActor*> OverlappingOilSpills;
			PrimComp->GetOverlappingActors(OverlappingOilSpills, AOilSpill::StaticClass());		
			//Count how many are on fire
			for (AActor* OilSpill : OverlappingOilSpills) {
				AOilSpill* OilSpillActor = Cast<AOilSpill>(OilSpill);
				if (OilSpillActor && OilSpillActor->IsOnFire) {
					NumberFireOnObjectif += 1;
				}
			}
		}		
	}

	float Damage = NumberOverLappingSlime * DamagePerSlimePerHalfSecond;
	if (NumberFireOnObjectif > 0) {
		Damage += NumberFireOnObjectif * DamagePerFirePerHalfSecond;
	}

	//Log number of slime and fire
	UE_LOG(LogTemp, Warning, TEXT("Number of slime: %d"), NumberOverLappingSlime);
	UE_LOG(LogTemp, Warning, TEXT("Number of fire: %d"), NumberFireOnObjectif);

	UpdateLife(Damage);
}

void ASlimeObjectif::OnLifeUpdated_Implementation()
{
}

void ASlimeObjectif::OnDeath_Implementation()
{
}

void ASlimeObjectif::HitByFireExplosion(float distanceFactor)
{
	if (!bTakeDamageFromFireAndExplosion) {
		return;
	}
	UpdateLife(FireExplosionDamage);	
}

void ASlimeObjectif::HitByElectricExplosion(float distanceFactor)
{
	if (!bTakeDamageFromFireAndExplosion) {
		return;
	}
	UpdateLife(ElectricExplosionDamage);
}

void ASlimeObjectif::UpdateLife(float damage)
{

	CurrentLife -= damage;
	CurrentLife = FMath::Clamp(CurrentLife, 0.0f, MaxLife);
	
	// Émettre l'événement sans paramètre
	OnHealthChanged.Broadcast();
	OnLifeUpdated();
	
	if (CurrentLife <= 0.0001f) {
		bIsDead = true;
		OnObjectifDeath.Broadcast(this);
		OnDeath();
	}	
}

// Called every frame
void ASlimeObjectif::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



