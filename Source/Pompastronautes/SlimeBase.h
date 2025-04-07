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
	Electric UMETA(DisplayName = "Electric"),
	Oil UMETA(DisplayName = "Oil")
};

UENUM(BlueprintType)
enum class EZoneEffectType : uint8
{
	WaterElectricExplosion   UMETA(DisplayName = "WaterElectricExplosion"),
	FireElectricExplosion  UMETA(DisplayName = "FireElectricExplosion"),
	ElecOilExplosion  UMETA(DisplayName = "ElecOilExplosion"),
	OilWaterExplosion  UMETA(DisplayName = "OilWaterExplosion"),
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ElecOilExplosionRadius = 250.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime")
	ESlimeType SlimeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime")
	TSubclassOf<class AOilDrop> OilDropClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime")
	float SameTypeSlimePropulsionForce = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime")
	float ElectricExplosionPropulsionForce = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime")
	float OilWaterExplosionPropulsionForce = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime")
	float OilWaterExplosionPropulsionRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime")
	bool IsGettingSucked = false;
	
	UPROPERTY(EditAnywhere, Category = "FX Slime")
	UNiagaraSystem* WaterElectricExplosionFX;

	UPROPERTY(EditAnywhere, Category = "FX Slime")
	UNiagaraSystem* OilElectricExplosionFX;
	
	UPROPERTY(EditAnywhere, Category = "FX Slime")
	UNiagaraSystem* EvaporationFX;

	UPROPERTY(EditAnywhere, Category = "FX Slime")
	UNiagaraSystem* OilWaterExplosionFX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ElecExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* FireExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* WaterOilExplosionSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundAttenuation* SoundAttenuation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TSubclassOf<UCameraShakeBase> ExplosionCameraShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float RadiusPlayerImpactByExplosion = 2000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OilSlime")
	int NumberOfOilDropsWater = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OilSlime")
	int NumberOfOilDropsElectric = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OilSlime")
	float OilWaterExplosionForce = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OilSlime")
	float OilElectricExplosionForce = 400.0f;

	
	
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
	void OilOnHitBySlime(ESlimeType OtherSlimeType, FVector& HitDirVector);

	void WaterOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, const FVector& SourcePosition);
	void ElectricOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, const FVector& SourcePosition);
	void OilOnAffectedByZoneEffect(EZoneEffectType ZoneEffectType, const FVector& SourcePosition);

	void WaterElecticityExplosion();

	void ElecOilExplosion();

	void OilWaterExplosion();

	//FX
	void PlayWaterElectricExplosionFX(float Delay, bool PlayAtLocation = false);
	void PlayOilElectricExplosionFX();
	void PlayEvaporationFX();
	void PlayOilWaterExplosionFX();

	//Sound
	void PlayElecExplosionSound();
	void PlayFireExplosionSound();
	void PlayWaterOilExplosionSound();

	//Shake
	void PlayExplosionCameraShake();

	void ImpactPlayersWithExplosion();
	

	FVector GetBounceDirection(FVector HitDirVector, FVector Normal);
	FVector GetExplosionPropulsion(FVector ExplosionSource, FVector Normal);

	void WakeUpControllerIfNeeded();

	void SpawnOilDrops(int NumberOfDrops, float OilDropDownwardBias, bool isOilOnFire, float OilDropExplosionForce);
};




