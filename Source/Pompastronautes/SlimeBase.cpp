// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeBase.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
ASlimeBase::ASlimeBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;	
}

// Called when the game starts or when spawned
void ASlimeBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASlimeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

inline void ASlimeBase::OnHitBySlime(ESlimeType OtherSlimeType, FVector HitDirVector)
{
	if (isExploding)
	{
		return;
	}	
	switch (SlimeType)
	{
		case ESlimeType::Water:
			WaterOnHitBySlime(OtherSlimeType, HitDirVector);
			break;
		case ESlimeType::Electric:
			ElectricOnHitBySlime(OtherSlimeType, HitDirVector);
			break;		
	}
}

void ASlimeBase::OnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, float DistFromSource)
{
	if (isExploding)
	{
		return;
	}
	
	switch (SlimeType)
	{
		case ESlimeType::Water:
			WaterOnAffectedByZoneEffect(ZoneEffectType, DistFromSource);
			break;
		case ESlimeType::Electric:
			ElectricOnAffectedByZoneEffect(ZoneEffectType, DistFromSource);
			break;				
	}
}

void ASlimeBase::WaterOnHitBySlime(ESlimeType OtherSlimeType, FVector HitDirVector)
{
	switch (OtherSlimeType)
	{
		case ESlimeType::Water:
			FVector BounceDirection = GetBounceDirection(HitDirVector, GetActorUpVector());
			HasToLaunchFromReaction = true;
			LaunchDirection = BounceDirection;			
			break;
		case ESlimeType::Electric:
			PlayWaterElectricExplosionFX(0.01f, true);
			WaterElecticityExplosion();
			break;
	}
}

void ASlimeBase::ElectricOnHitBySlime(ESlimeType OtherSlimeType, FVector HitDirVector)
{
	switch (OtherSlimeType)
	{
	case ESlimeType::Water:
		UE_LOG(LogTemp, Warning, TEXT("WaterOnHitBySlime Water"));
		break;
	case ESlimeType::Electric:
		FVector BounceDirection = GetBounceDirection(HitDirVector, GetActorUpVector());
		HasToLaunchFromReaction = true;
		LaunchDirection = BounceDirection;		
		break;
	}
}

void ASlimeBase::WaterOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, float DistFromSource)
{
	float Delay = 0.0f;
	switch (ZoneEffectType)
	{
		case EZoneEffectType::WaterElectricExplosion:
			//Explode after a delay based on distance from source
			Delay = FMath::Lerp(minExplosionDelay, maxExplosionDelay, DistFromSource / ExplosionRadius);
			GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &ASlimeBase::WaterElecticityExplosion, Delay, false);
			PlayWaterElectricExplosionFX(Delay);
			break;
		case EZoneEffectType::FireElectricExplosion:
			UE_LOG(LogTemp, Warning, TEXT("WaterOnAffectedByZoneEffect FireElectricExplosion NOT IMPLEMENTED"));
			break;		
	}	
	
}

void ASlimeBase::ElectricOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, float DistFromSource)
{
	
}


void ASlimeBase::WaterElecticityExplosion()
{ 
	TArray<FHitResult> HitResults;

	// Define collision query params, etc. (omitted for brevity)
	// For demonstration, let's do a simple sphere overlap:
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(ExplosionRadius);

	bool bHit = GetWorld()->SweepMultiByObjectType(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + FVector::UpVector * 0.1f, // minimal movement
		FQuat::Identity,
		ECC_GameTraceChannel2, // Slime Object Type
		CollisionShape
	);
	
	FVector ActorLocation = GetActorLocation();
	float distanceToActor =0.0f;

	if (bHit)
	{
		for (auto& Hit : HitResults)
		{
			ASlimeBase* OtherSlime = Cast<ASlimeBase>(Hit.GetActor());
			//Log hit distance
			UE_LOG(LogTemp, Warning, TEXT("WaterElecticityExplosion Hit Distance: %f"), Hit.Distance);
			if (OtherSlime)
			{
				distanceToActor = FVector::Dist(ActorLocation, OtherSlime->GetActorLocation());
				OtherSlime->OnAffectedByZoneEffect(EZoneEffectType::WaterElectricExplosion,distanceToActor);
			}
		}
	}
	
	Destroy();
}

void ASlimeBase::PlayWaterElectricExplosionFX(float Delay, bool PlayAtLocation)
{
	if (WaterElectricExplosionFX)
	{
		// Get the root component's location
		USceneComponent* SlimeRoot = GetRootComponent();
		
		UNiagaraComponent* NiagaraComp;
		if (PlayAtLocation)
		{
			NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WaterElectricExplosionFX, SlimeRoot->GetComponentLocation(), FRotator(0.f), FVector(1.f), true, true, ENCPoolMethod::AutoRelease);
		}
		else
		{
			NiagaraComp	= UNiagaraFunctionLibrary::SpawnSystemAttached(WaterElectricExplosionFX, SlimeRoot, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);
		}
		// Parameters can be set like this (see documentation for further info) - the names and type must match the user exposed parameter in the Niagara System
		NiagaraComp->SetNiagaraVariableFloat(FString("Burst Delay"), Delay);
		NiagaraComp->SetNiagaraVariableFloat(FString("Sphere Radius"), ExplosionRadius);
	}
}

FVector ASlimeBase::GetBounceDirection(FVector HitDirVector, FVector Normal)
{
	//FVector BounceDirection = FVector::ZeroVector;
	FVector HitNormal = Normal;
	FVector HitDir = HitDirVector;

	FVector BounceDirection =   HitDirVector - 2.0f * (FVector::DotProduct(HitDirVector, Normal)) * Normal;
	BounceDirection*= -1.0f;
	BounceDirection.Normalize();

	//Line trace to check if the bounce direction is valid
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + BounceDirection * 40.0f;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1);

	if (bHit)
	{
		BounceDirection = Normal;
		BounceDirection.Normalize();
	}
	
	return BounceDirection;
}


