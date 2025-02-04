// Fill out your copyright notice in the Description page of Project Settings.


#include "SlimeNavGridTracer.h"
#include "Components/SphereComponent.h"

// Sets default values
ASlimeNavGridTracer::ASlimeNavGridTracer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	CollisionComp->SetCollisionObjectType(ECC_WorldStatic);
	CollisionComp->BodyInstance.SetCollisionProfileName("Custom");
	CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionComp->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics, true);
	CollisionComp->SetSphereRadius(10.f);
	RootComponent = CollisionComp;

}

// Called when the game starts or when spawned
void ASlimeNavGridTracer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASlimeNavGridTracer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

