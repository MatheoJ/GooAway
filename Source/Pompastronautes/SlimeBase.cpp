// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeBase.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "SlimeAiController.h"
#include "OilDrop.h"

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
		case ESlimeType::Oil:
			OilOnHitBySlime(OtherSlimeType, HitDirVector);
			break;
	}
}

void ASlimeBase::OnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, FVector& SourcePosition)
{
	if (isExploding)
	{
		return;
	}
	
	switch (SlimeType)
	{
		case ESlimeType::Water:
			WaterOnAffectedByZoneEffect(ZoneEffectType, SourcePosition);
			break;
		case ESlimeType::Electric:
			ElectricOnAffectedByZoneEffect(ZoneEffectType, SourcePosition);
			break;
		case ESlimeType::Oil:
			OilOnAffectedByZoneEffect(ZoneEffectType, SourcePosition);
			break;
	}
}

void ASlimeBase::WaterOnHitBySlime(ESlimeType OtherSlimeType, FVector& HitDirVector)
{
	ASlimeAiController* SlimeAiController;
	switch (OtherSlimeType)
	{
		case ESlimeType::Water:
			FVector BounceDirection = GetBounceDirection(HitDirVector, GetActorUpVector());
			WakeUpControllerIfNeeded();
			//HasToLaunchFromReaction = true;
			//LaunchDirection = BounceDirection;
			SlimeAiController = Cast<ASlimeAiController>(GetController());
			if (SlimeAiController)
			{
				SlimeAiController->LaunchSlimeInDirection(BounceDirection);
			}
			break;
		case ESlimeType::Electric:
			PlayWaterElectricExplosionFX(0.01f, true);
			WaterElecticityExplosion();
			break;
		case ESlimeType::Oil:
			UE_LOG(LogTemp, Warning, TEXT("WaterOnHitBySlime Oil NOT IMPLEMENTED"));
			break;
	}
}

void ASlimeBase::ElectricOnHitBySlime(ESlimeType OtherSlimeType, FVector& HitDirVector)
{
	ASlimeAiController* SlimeAiController;
	
	switch (OtherSlimeType)
	{
	case ESlimeType::Water:
		PlayWaterElectricExplosionFX(0.01f, true);
		WaterElecticityExplosion();
		break;
	case ESlimeType::Electric:
		FVector BounceDirection = GetBounceDirection(HitDirVector, GetActorUpVector());
		WakeUpControllerIfNeeded();
		//HasToLaunchFromReaction = true;
		//LaunchDirection = BounceDirection;
		SlimeAiController = Cast<ASlimeAiController>(GetController());
		if (SlimeAiController)
		{
			SlimeAiController->LaunchSlimeInDirection(BounceDirection);
		}		
		break;
	case ESlimeType::Oil:
		UE_LOG(LogTemp, Warning, TEXT("ElectricOnHitBySlime Oil NOT IMPLEMENTED"));
		break;
	}
}

void ASlimeBase::OilOnHitBySlime(ESlimeType OtherSlimeType, FVector& HitDirVector)
{
	ASlimeAiController* SlimeAiController;
	
	switch (OtherSlimeType)
	{
		case ESlimeType::Water:
			SpawnOilDrops(10, 0.2f, false);
			Destroy();
			break;
		case ESlimeType::Electric:
			SpawnOilDrops(10, 0.05f, true);
			Destroy();
			break;
		case ESlimeType::Oil:
			FVector BounceDirection = GetBounceDirection(HitDirVector, GetActorUpVector());
			WakeUpControllerIfNeeded();
			//HasToLaunchFromReaction = true;
			//LaunchDirection = BounceDirection;
			SlimeAiController = Cast<ASlimeAiController>(GetController());
			if (SlimeAiController)
			{
				SlimeAiController->LaunchSlimeInDirection(BounceDirection);
			}		
			break;
		}
}

void ASlimeBase::WaterOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, const FVector& SourcePosition)
{
	float Delay = 0.0f;
	float DistFromSource = FVector::Dist(GetActorLocation(), SourcePosition);
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

void ASlimeBase::ElectricOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, const FVector& SourcePosition)
{
	ASlimeAiController* SlimeAiController;
	
	switch (ZoneEffectType)
	{
		case EZoneEffectType::WaterElectricExplosion:
			FVector Propulsion = GetExplosionPropulsion(SourcePosition, GetActorUpVector());
			if (Propulsion != FVector::ZeroVector)
			{
				WakeUpControllerIfNeeded();
				/*HasToLaunchFromReaction = true;
				LaunchDirection = Propulsion;*/
				SlimeAiController = Cast<ASlimeAiController>(GetController());
				if (SlimeAiController)
				{
					SlimeAiController->LaunchSlimeInDirection(Propulsion);
				}
			}
			break;
		case EZoneEffectType::FireElectricExplosion:
			UE_LOG(LogTemp, Warning, TEXT("ElectricOnAffectedByZoneEffect FireElectricExplosion NOT IMPLEMENTED"));
			break;
	}
}

void ASlimeBase::OilOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, const FVector& SourcePosition)
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
	
	if (bHit)
	{
		for (auto& Hit : HitResults)
		{
			ASlimeBase* OtherSlime = Cast<ASlimeBase>(Hit.GetActor());
			if (OtherSlime)
			{
				FVector ActorLocation = GetActorLocation();
				OtherSlime->OnAffectedByZoneEffect(EZoneEffectType::WaterElectricExplosion,ActorLocation);
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
		float timeToLeaveForExplosion = 0.3;

		if (PlayAtLocation)
		{
			timeToLeaveForExplosion = 0.0f;
		}
		
		NiagaraComp->SetVariableFloat(FName("Burst Delay"), Delay-timeToLeaveForExplosion);
		NiagaraComp->SetVariableFloat(FName("Sphere Radius"), ExplosionRadius);
	}
}

FVector ASlimeBase::GetBounceDirection(FVector HitDirVector, FVector Normal)
{
	//FVector BounceDirection = FVector::ZeroVector;
	FVector HitNormal = Normal;
	FVector HitDir = HitDirVector;

	FVector BounceDirection =   HitDirVector - 2.0f * (FVector::DotProduct(HitDirVector, Normal)) * Normal;
	//BounceDirection*= -1.0f;
	BounceDirection.Normalize();

	//Line trace to check if the bounce direction is valid
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + BounceDirection * 40.0f;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1);

	if (bHit)
	{
		BounceDirection = Normal;
		BounceDirection.Normalize();
	}
	
	return BounceDirection;
}

FVector ASlimeBase::GetExplosionPropulsion(FVector ExplosionSource, FVector Normal)
{
	FVector direction = GetActorLocation() - ExplosionSource;
	direction.Normalize();

	float distFromsource = FVector::Dist(GetActorLocation(), ExplosionSource);
	float minNormalContribution = 0.2f;
	float normalContribution = FMath::Lerp(1.0f, minNormalContribution, distFromsource / ExplosionRadius);

	direction += normalContribution * Normal;
	direction.Normalize();

	//Line trace to check if the direction is valid
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + direction * 50.0f;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1);

	if (bHit)
	{
		return FVector::Zero();
	}	
	
	return direction;
}

void ASlimeBase::WakeUpControllerIfNeeded()
{
	//Check if the controller tick is desactivated and wake it if needed

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}

	if (GetAttachParentActor() != nullptr)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	if (!AIController->IsActorTickEnabled())
	{
		UBrainComponent* brainComp = AIController->GetBrainComponent();
		if (brainComp)
		{
			brainComp->RestartLogic();
		}		
		AIController->SetActorTickEnabled(true);
	}	
}


void ASlimeBase::SpawnOilDrops(int NumberOfDrops, float OilDropDownwardBias, bool isOilOnFire)
{
	float OilDropSpawnHeight = 30.0f;
	float OilDropExplosionForce = 400.0f;
	    
    FVector SlimeLocation = GetActorLocation();
    FVector SlimeUpVector = GetActorUpVector();
    
    // Spawn position is above the slime along its up vector
    FVector SpawnPosition = SlimeLocation + (SlimeUpVector * OilDropSpawnHeight);
    
    // Direction toward the surface is opposite of up vector
    FVector SurfaceDirection = -SlimeUpVector;
    
    for (int i = 0; i < NumberOfDrops; i++)
    {
        // Start with a completely random direction
        FVector RandomDirection = FMath::VRand();
        
        // Blend between random direction and surface direction based on downward bias
        FVector LaunchDirection = FMath::Lerp(RandomDirection, SurfaceDirection, OilDropDownwardBias);
        LaunchDirection.Normalize();
        
        // Spawn the oil drop
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		//DrawDebugLine(GetWorld(), SpawnPosition, SpawnPosition + LaunchDirection * 100.0f, FColor::Red, true, 5.0f, 0, 5.0f);
    	
        AOilDrop* OilDrop = GetWorld()->SpawnActor<AOilDrop>(
            OilDropClass,
            SpawnPosition,
            FRotator::ZeroRotator,
            SpawnParams
        );
        
        if (OilDrop)
        {
            // Get the physics component (assuming it has one)
            UPrimitiveComponent* PhysicsComponent = Cast<UPrimitiveComponent>(OilDrop->GetRootComponent());
            if (PhysicsComponent)
            {
                float RandomForce = OilDropExplosionForce * (0.8f + FMath::FRand() * 0.4f); // 80-120% of base force
                PhysicsComponent->AddImpulse(LaunchDirection * RandomForce, NAME_None, true);
            }

        	if (isOilOnFire)
			{
				OilDrop->SetOnFire();
			}
        }
    }
}

