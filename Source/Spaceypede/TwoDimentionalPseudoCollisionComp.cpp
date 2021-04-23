// Fill out your copyright notice in the Description page of Project Settings.


#include "TwoDimentionalPseudoCollisionComp.h"

// Sets default values for this component's properties
UTwoDimentionalPseudoCollisionComp::UTwoDimentionalPseudoCollisionComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	EditorVisualMarkers.Init(nullptr, 4);
	EditorVisualMarkers[0]=CreateDefaultSubobject<USphereComponent>(TEXT("VisualMarkerN"));
	EditorVisualMarkers[1]=CreateDefaultSubobject<USphereComponent>(TEXT("VisualMarkerS"));
	EditorVisualMarkers[2]=CreateDefaultSubobject<USphereComponent>(TEXT("VisualMarkerE"));
	EditorVisualMarkers[3]=CreateDefaultSubobject<USphereComponent>(TEXT("VisualMarkerW"));
	for (USphereComponent* i : EditorVisualMarkers) {
		i->SetupAttachment(this);
		i->InitSphereRadius(15.0f);
	}
	EditorOnlyMoveMarkers();
}

// Called when the game starts
void UTwoDimentionalPseudoCollisionComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTwoDimentionalPseudoCollisionComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTwoDimentionalPseudoCollisionComp::EditorOnlyMoveMarkers()
{
	if (EditorVisualMarkers.Num() == 4) {
		EditorVisualMarkers[0]->SetRelativeLocation(FVector(lengthFromCenter, 0.0f, 0.0f));
		EditorVisualMarkers[1]->SetRelativeLocation(FVector(lengthFromCenter * -1, 0.0f, 0.0f));
		EditorVisualMarkers[2]->SetRelativeLocation(FVector(0.0f, widthFromCenter, 0.0f));
		EditorVisualMarkers[3]->SetRelativeLocation(FVector(0.0f, widthFromCenter * -1, 0.0f));
	}
}

bool UTwoDimentionalPseudoCollisionComp::IsOverlappingTwoDimentional()
{
	return false;
}

