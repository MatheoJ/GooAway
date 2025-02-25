// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SlimeBase.generated.h"

class UNiagaraSystem;


UENUM(BlueprintType)
enum class ESlimeType : uint8
{
	Water   UMETA(DisplayName = "Water"),
	Electric UMETA(DisplayName = "Electric")	
};

UENUM(BlueprintType)
enum class EZoneEffectType : uint8
{
	WaterElectricExplosion   UMETA(DisplayName = "WaterElectricExplosion"),
	FireElectricExplosion  UMETA(DisplayName = "FireElectricExplosion")	
};

UCLASS()
class POMPASTRONAUTES_API ASlimeBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASlimeBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float minExplosionDelay = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float maxExplosionDelay = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionRadius = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launch")
	bool HasToLaunchFromReaction = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launch")
	FVector LaunchDirection = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime")
	ESlimeType SlimeType;

	UPROPERTY(EditAnywhere, Category = "FX Slime")
	UNiagaraSystem* WaterElectricExplosionFX;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when another slime collides with this one
	UFUNCTION(BlueprintCallable, Category = "Slime")
	virtual void OnHitBySlime(ESlimeType OtherSlimeType, FVector HitDirVector = FVector::ZeroVector);

	// Called when a zone effect (like an electric explosion) reaches this slime
	UFUNCTION(BlueprintCallable, Category = "Slime")
	virtual void OnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, FVector& SourcePosition);

private:
	bool isExploding = false;

	FTimerHandle ExplosionTimerHandle;
	
	void WaterOnHitBySlime(ESlimeType OtherSlimeType, FVector& HitDirVector);
	void ElectricOnHitBySlime(ESlimeType OtherSlimeType, FVector& HitDirVector);

	void WaterOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, const FVector& SourcePosition);
	void ElectricOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, const FVector& SourcePosition);

	void WaterElecticityExplosion();

	//FX
	void PlayWaterElectricExplosionFX(float Delay, bool PlayAtLocation = false);

	FVector GetBounceDirection(FVector HitDirVector, FVector Normal);
	FVector GetExplosionPropulsion(FVector ExplosionSource, FVector Normal);

	void WakeUpControllerIfNeeded();
};




