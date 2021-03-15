// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Containers/Queue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"

#include "PedeSegment.generated.h"

using namespace std;


UCLASS()
class SPACEYPEDE_API APedeSegment : public APawn
{
	GENERATED_BODY()

		

public:
	// Sets default values for this pawn's properties
	APedeSegment();

	class TQueue<FVector, EQueueMode::Mpsc> FVectorTrailQueue;
	//TODO location+rotation

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* MainMeshComponent;

	//TODO ActionComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* RotatingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* TopViewCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* TopSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionNose;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionButt;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionContactDamage;

	class APedeSegment* Leads;

	class APedeSegment* LedBy;

	/*methods:
	* connectTo(PedeSegment* otherSegment) - if(otherSegment.getbIsRogue) generates an initial trailQueue, otherSegment.setLeads(this), calls passControl
	* setLedBy(PedeSegment* LedBy)
	* getLedBy()
	* setLeads(PedeSegment* Leads)
	* getLeads()
	* getbIsRogue()
	* passControl(PedeSegment* otherSegment) - unbinds player controlled and camera, this.bIsPossessed = false, calls otherSegment.recieveControl(PlayerController?)
	* recieveControl(PlayerController?) - binds player controlled and camera, clears up trailQueue, removes dependancy LedBy, bIsPossessed = true, bIsRogue = false
	* disconnect() - LedBy.setLeads(null), clean up trailQueue, if (!this.bIsPossessed) set this and the rest of segments rogue=true (or disconnect?)
	* 
	* move() - all move logic each frame
	* turn()
	* forward()
	* followTrail()
	* leaveTrail()
	*/

protected:

	UPROPERTY(VisibleAnywhere)
		bool bIsPossessed; //possessed by a player

	UPROPERTY(VisibleAnywhere)
		bool bIsRogue;  //no = belongs to a controlled train

	UPROPERTY(EditAnywhere, BlueprintReadOnly) //TODO change to visible only
		float baseSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float speedBoostModifier; //multiplier for accelerated movement (usually gets set to 2.0)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float speedHaltModifier; //multiplier for halting (usually gets set to 0)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float turnMinCircleRadius;

	

private:

	UFUNCTION()
	void SetTurnDirection(float value);

	UPROPERTY()
	float turnDirection;

	UFUNCTION()
	void BoostSpeed();

	UFUNCTION()
	void DeboostSpeed();

	UFUNCTION()
	void Halt();

	UFUNCTION()
	void UnHalt();

	UFUNCTION()
	void moveAndLeaveTrail(float DeltaTime); //every tick moves and records new location for next-in-line segment

	UFUNCTION()
	float CalculateSpeed(); //uses baseSpeed, speed modifiers and properties of segments attached to give final movement speed

	UFUNCTION()
	float CalculateTurnAngle(float distanceTraveledInCircle); //calculates the angle of a circle segment with radius turnMinCircleRadius that is passed for distance traveled

	UFUNCTION()
	float CalculateDistanceCutThroughCircle(float turnAngle); //calculates distance needed to travel when cutting trough the circle to achieve same distance as distanceTraveledInCircle

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
