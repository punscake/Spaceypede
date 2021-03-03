// Fill out your copyright notice in the Description page of Project Settings.


#include "PedeSegment.h"

// Sets default values
APedeSegment::APedeSegment()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

}

// Called when the game starts or when spawned
void APedeSegment::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APedeSegment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APedeSegment::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

