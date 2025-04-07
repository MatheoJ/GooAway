// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeBase.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "CharacterInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "SlimeAiController.h"
#include "OilDrop.h"
#include "SlimeObjectif.h"
#include "Kismet/GameplayStatics.h"

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
				SlimeAiController->LaunchSlimeInDirection(BounceDirection, SameTypeSlimePropulsionForce);
			}
			break;
		case ESlimeType::Electric:
			PlayWaterElectricExplosionFX(0.01f, true);
			WaterElecticityExplosion();
			break;
		case ESlimeType::Oil:
			SpawnOilDrops(NumberOfOilDropsWater, 0.2f, false, OilWaterExplosionForce);
			OilWaterExplosion();
			Destroy();
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
			SlimeAiController->LaunchSlimeInDirection(BounceDirection, SameTypeSlimePropulsionForce);
		}		
		break;
	case ESlimeType::Oil:
		SpawnOilDrops(NumberOfOilDropsElectric, 0.05f, true, OilElectricExplosionForce);
		ElecOilExplosion();
		Destroy();
		break;
	}
}

void ASlimeBase::OilOnHitBySlime(ESlimeType OtherSlimeType, FVector& HitDirVector)
{
	ASlimeAiController* SlimeAiController;
	
	switch (OtherSlimeType)
	{
		case ESlimeType::Water:
			SpawnOilDrops(NumberOfOilDropsWater, 0.2f, false, OilWaterExplosionForce);
			OilWaterExplosion();
			Destroy();
			break;
		case ESlimeType::Electric:
			SpawnOilDrops(NumberOfOilDropsElectric, 0.05f, true, OilElectricExplosionForce);
			ElecOilExplosion();
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
				SlimeAiController->LaunchSlimeInDirection(BounceDirection, SameTypeSlimePropulsionForce);
			}		
			break;
		}
}

void ASlimeBase::WaterOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, const FVector& SourcePosition)
{
	ASlimeAiController* SlimeAiController;
	float Delay = 0.0f;
	float DistFromSource = FVector::Dist(GetActorLocation(), SourcePosition);
	switch (ZoneEffectType)
	{
		case EZoneEffectType::WaterElectricExplosion:
			//Explode after a delay based on distance from source
			Delay = FMath::Lerp(minExplosionDelay, maxExplosionDelay, DistFromSource / ExplosionRadius);
			GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &ASlimeBase::WaterElecticityExplosion, Delay, false);
			PlayWaterElectricExplosionFX(Delay);
			isExploding = true;
			break;
		case EZoneEffectType::FireElectricExplosion:
			UE_LOG(LogTemp, Warning, TEXT("WaterOnAffectedByZoneEffect FireElectricExplosion NOT IMPLEMENTED"));
			break;
		case EZoneEffectType::ElecOilExplosion:
			PlayEvaporationFX();	
			Destroy();
			break;
		case EZoneEffectType::OilWaterExplosion:
			FVector Propulsion2 = GetExplosionPropulsion(SourcePosition, GetActorUpVector());
			if (Propulsion2 != FVector::ZeroVector)
			{
				WakeUpControllerIfNeeded();
				SlimeAiController = Cast<ASlimeAiController>(GetController());
				if (SlimeAiController)
				{
					SlimeAiController->LaunchSlimeInDirection(Propulsion2, OilWaterExplosionPropulsionForce);
				}
			}
			break;
	}	
	
}

void ASlimeBase::ElectricOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, const FVector& SourcePosition)
{
	ASlimeAiController* SlimeAiController;
	float Delay = 0.0f;
	float DistFromSource = FVector::Dist(GetActorLocation(), SourcePosition);
	
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
					SlimeAiController->LaunchSlimeInDirection(Propulsion, ElectricExplosionPropulsionForce);
				}
			}
			break;
		case EZoneEffectType::FireElectricExplosion:
			UE_LOG(LogTemp, Warning, TEXT("ElectricOnAffectedByZoneEffect FireElectricExplosion NOT IMPLEMENTED"));
			break;
		case EZoneEffectType::ElecOilExplosion:
			Delay = FMath::Lerp(0.1, 0.4, DistFromSource / ElecOilExplosionRadius);
			GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &ASlimeBase::WaterElecticityExplosion, Delay, false);
			isExploding = true;
			break;
		case EZoneEffectType::OilWaterExplosion:
			FVector Propulsion2 = GetExplosionPropulsion(SourcePosition, GetActorUpVector());
			if (Propulsion2 != FVector::ZeroVector)
			{
				WakeUpControllerIfNeeded();
				SlimeAiController = Cast<ASlimeAiController>(GetController());
				if (SlimeAiController)
				{
					SlimeAiController->LaunchSlimeInDirection(Propulsion2, OilWaterExplosionPropulsionForce);
				}
			}
			break;
	}
}

void ASlimeBase::OilOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, const FVector& SourcePosition)
{
	float Delay = 0.0f;
	float DistFromSource = FVector::Dist(GetActorLocation(), SourcePosition);
	ASlimeAiController* SlimeAiController;
	FTimerHandle OilDropsTimerHandle;

	switch (ZoneEffectType)
	{
		case EZoneEffectType::WaterElectricExplosion:
			UE_LOG(LogTemp, Warning, TEXT("OilOnAffectedByZoneEffect WaterElectricExplosion NOT IMPLEMENTED"));
		case EZoneEffectType::FireElectricExplosion:
			UE_LOG(LogTemp, Warning, TEXT("OilOnAffectedByZoneEffect FireElectricExplosion NOT IMPLEMENTED"));
		case EZoneEffectType::ElecOilExplosion:
			Delay = FMath::Lerp(0.1, 0.4, DistFromSource / ElecOilExplosionRadius);
			GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &ASlimeBase::ElecOilExplosion, Delay, false);			
			GetWorldTimerManager().SetTimer(OilDropsTimerHandle, [this]() {
				SpawnOilDrops(NumberOfOilDropsElectric, 0.05f, true, OilElectricExplosionForce);
			}, Delay, false);
			isExploding = true;
			break;
		case EZoneEffectType::OilWaterExplosion:
			FVector Propulsion2 = GetExplosionPropulsion(SourcePosition, GetActorUpVector());
			if (Propulsion2 != FVector::ZeroVector)
			{
				WakeUpControllerIfNeeded();
				SlimeAiController = Cast<ASlimeAiController>(GetController());
				if (SlimeAiController)
				{
					SlimeAiController->LaunchSlimeInDirection(Propulsion2, OilWaterExplosionPropulsionForce);
				}
			}
			break;
	}	
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

	bHit = GetWorld()->SweepMultiByObjectType(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + FVector::UpVector * 0.1f, // minimal movement
		FQuat::Identity,
		ECC_GameTraceChannel8, 
		CollisionShape
	);

	if (bHit)
	{
		for (auto& Hit : HitResults)
		{
			ASlimeObjectif* objectif = Cast<ASlimeObjectif>(Hit.GetActor());
			if (objectif)
			{
				objectif->HitByElectricExplosion(0.0f);
				break;
			}
		}
	}

	
	Destroy();
}

void ASlimeBase::ElecOilExplosion()
{
	TArray<FHitResult> HitResults;

	// Define collision query params, etc. (omitted for brevity)
	// For demonstration, let's do a simple sphere overlap:
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(ElecOilExplosionRadius);

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
			if (Hit.GetActor() == this)
			{
				continue;
			}
			
			ASlimeBase* OtherSlime = Cast<ASlimeBase>(Hit.GetActor());
			if (OtherSlime)
			{
				FVector ActorLocation = GetActorLocation();
				OtherSlime->OnAffectedByZoneEffect(EZoneEffectType::ElecOilExplosion,ActorLocation);
			}
		}
	}

	bHit = GetWorld()->SweepMultiByObjectType(
	HitResults,
	GetActorLocation(),
	GetActorLocation() + FVector::UpVector * 0.1f, // minimal movement
	FQuat::Identity,
	ECC_GameTraceChannel8, 
	CollisionShape
	);

	if (bHit)
	{
		for (auto& Hit : HitResults)
		{
			ASlimeObjectif* objectif = Cast<ASlimeObjectif>(Hit.GetActor());
			if (objectif)
			{
				objectif->HitByFireExplosion(0.0f);
				break;
			}
		}
	}

	
	PlayOilElectricExplosionFX();
	Destroy();
}

void ASlimeBase::OilWaterExplosion()
{
	TArray<FHitResult> HitResults;

	// Define collision query params, etc. (omitted for brevity)
	// For demonstration, let's do a simple sphere overlap:
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(OilWaterExplosionPropulsionRadius);

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
			if (Hit.GetActor() == this)
			{
				continue;
			}
			
			ASlimeBase* OtherSlime = Cast<ASlimeBase>(Hit.GetActor());
			if (OtherSlime)
			{
				FVector ActorLocation = GetActorLocation();
				OtherSlime->OnAffectedByZoneEffect(EZoneEffectType::OilWaterExplosion,ActorLocation);
			}
		}
	}

	PlayOilWaterExplosionFX();
}

void ASlimeBase::PlayWaterElectricExplosionFX(float Delay, bool PlayAtLocation)
{
	PlayElecExplosionSound();
	PlayExplosionCameraShake();
	ImpactPlayersWithExplosion();
	
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

void ASlimeBase::PlayOilElectricExplosionFX()
{
	PlayExplosionCameraShake();
	PlayFireExplosionSound();
	if (OilElectricExplosionFX)
	{
		// play at location
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), OilElectricExplosionFX, GetActorLocation(), GetActorRotation(), FVector(1.f), true, true, ENCPoolMethod::AutoRelease);
		NiagaraComp->SetVariableFloat(FName("Burst Scale"), ElecOilExplosionRadius);
	}
}

void ASlimeBase::PlayEvaporationFX()
{
	if (EvaporationFX)
	{
		// play at location
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EvaporationFX, GetActorLocation(), FRotator(0.f), FVector(1.f), true, true, ENCPoolMethod::AutoRelease);
	}
}

void ASlimeBase::PlayOilWaterExplosionFX()
{
	if (OilWaterExplosionFX)
	{
		// play at location
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), OilWaterExplosionFX, GetActorLocation(), GetActorRotation(), FVector(1.f), true, true, ENCPoolMethod::AutoRelease);
	}
}

void ASlimeBase::PlayElecExplosionSound()
{
	if (ElecExplosionSound)
	{
		float Pitch = FMath::RandRange(0.8f, 1.2f);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ElecExplosionSound, GetActorLocation(), 0.15f, Pitch, 0.0f, SoundAttenuation);
	}
}

void ASlimeBase::PlayFireExplosionSound()
{
	if ( FireExplosionSound)
	{
		//Get random pitch between 0.8 and 1.2
		float Pitch = FMath::RandRange(0.8f, 1.2f);
		
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireExplosionSound, GetActorLocation(), 0.4f, Pitch, 0.0f, SoundAttenuation);
	}
}

void ASlimeBase::PlayExplosionCameraShake()
{
	if (ExplosionCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(
			GetWorld(),
			ExplosionCameraShake,
			GetActorLocation(),
			300.0f,  // Inner radius
			1700.0f, // Outer radius
			1.0f,    // Falloff
			false    // Orient shake towards epicenter
		);
	}
}

void ASlimeBase::ImpactPlayersWithExplosion()
{
	// Get all players
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterInterface::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		ACharacterInterface* Character = Cast<ACharacterInterface>(Actor);
		if (Character)
		{
			FVector ActorLocation = Actor->GetActorLocation();
			float Dist = FVector::Dist(ActorLocation, GetActorLocation());
			if (Dist < RadiusPlayerImpactByExplosion)
			{
				float factorDist = 1.0f - (Dist / RadiusPlayerImpactByExplosion);
				Character->TouchByExplosion(factorDist);
			}
		}
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


void ASlimeBase::SpawnOilDrops(int NumberOfDrops, float OilDropDownwardBias, bool isOilOnFire, float OilDropExplosionForce)
{
	float OilDropSpawnHeight = 30.0f;
	    
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

