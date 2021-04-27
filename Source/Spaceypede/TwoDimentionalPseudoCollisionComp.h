// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Containers/Array.h"
#include "Components/SphereComponent.h"
#include "BasicTwoDShapesCollisionHelper.h"

#include "TwoDimentionalPseudoCollisionComp.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEYPEDE_API UTwoDimentionalPseudoCollisionComp : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTwoDimentionalPseudoCollisionComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:

	UPROPERTY(EditAnywhere)
		TEnumAsByte<FEnumTypeOfHitbox> TypeOfHitbox;

	UPROPERTY()
		TArray<USphereComponent*> EditorVisualMarkers;

	UPROPERTY(EditAnywhere)
		float lengthFromCenter; //"a" for ellipses, half-length for rectangles

	UPROPERTY(EditAnywhere)
		float widthFromCenter; //"b" for ellipses, half-width for rectangles

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//virtual void OnConstruction(const FTransform& Transform) override;

	//virtual void PostEditChangeProperty (FPropertyChangedEvent& PropertyChangedEvent) override;

#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
	{
		//FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
		EditorOnlyMoveMarkers();
	}
#endif

	UFUNCTION()
		void EditorOnlyMoveMarkers();

	UFUNCTION()
		bool IsOverlappingTwoDimentional();

	//UFUNCTION()

};