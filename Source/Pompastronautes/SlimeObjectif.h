// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Delegates/DelegateCombinations.h" 
#include "SlimeObjectif.generated.h"

// Déclaration d'un Delegate Dynamique sans paramètre
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSlimeObjectifDeathDelegate, ASlimeObjectif*, DeadObjectif);


UCLASS()
class POMPASTRONAUTES_API ASlimeObjectif : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlimeObjectif();

	UFUNCTION(BlueprintCallable)
	FVector GetNextAttackPoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeObjectif")
	bool canTakeDamage = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeObjectif")
	float MaxLife = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeObjectif")
	float CurrentLife;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeObjectif")
	float DamagePerSlimePerHalfSecond = 0.005f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeObjectif")
	float DamagePerFirePerHalfSecond = 0.04f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeObjectif")
	float ElectricExplosionDamage = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeObjectif")
	float FireExplosionDamage = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeObjectif")
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlimeObjectif")
	bool bTakeDamageFromFireAndExplosion = false;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FSlimeObjectifDeathDelegate OnObjectifDeath;

	UFUNCTION(BlueprintCallable, Category="SlimeObjectif")
	void HitByFireExplosion(float distanceFactor);

	UFUNCTION(BlueprintCallable, Category="SlimeObjectif")
	void HitByElectricExplosion(float distanceFactor);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void FindAttackPoints();

	TArray<FVector> AttackPoints;

	
	int CurrentAttackPointIndex = 0;

	UPROPERTY()
	TArray<UShapeComponent*> CollisionComponents;

	UFUNCTION()
	void UpdateLifeWithAttackingSlime();

	FTimerHandle TimerHandle_CheckForEnemies;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="SlimeObjectif")
	void OnLifeUpdated();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="SlimeObjectif")
	void OnDeath();



	void UpdateLife(float damage);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

 
