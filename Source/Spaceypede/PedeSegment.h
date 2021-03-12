// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Containers/Queue.h"
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

	//TODO mesh

	//TODO ActionComponent

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionNose;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionButt;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionContactDamage;

	class APedeSegment* Leads;

	class APedeSegment* LedBy;

	/*methods:
	* connectTo(PedeSegment* otherSegment) - if(otherSegment.getIsRogue) generates an initial trailQueue, otherSegment.setLeads(this), calls passControl
	* setLedBy(PedeSegment* LedBy)
	* getLedBy()
	* setLeads(PedeSegment* Leads)
	* getLeads()
	* getIsRogue()
	* passControl(PedeSegment* otherSegment) - unbinds player controlled and camera, this.isPossessed = false, calls otherSegment.recieveControl(PlayerController?)
	* recieveControl(PlayerController?) - binds player controlled and camera, clears up trailQueue, removes dependancy LedBy, isPossessed = true, isRogue = false
	* disconnect() - LedBy.setLeads(null), clean up trailQueue, if (!this.isPossessed) set this and the rest of segments rogue=true (or disconnect?)
	* 
	* move() - all move logic each frame
	* turn()
	* forward()
	* followTrail()
	* leaveTrail()
	*/

protected:

	UPROPERTY(VisibleAnywhere)
		bool isPossessed; //possessed by a player

	UPROPERTY(VisibleAnywhere)
		bool isRogue;  //no = belongs to a controlled train

	UPROPERTY(EditAnywhere, BlueprintReadOnly) //TODO change to visible only
		float baseSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float speedBoostModifier; //multiplier for accelerated movement (usually gets set to 2.0)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float speedHaltModifier; //multiplier for halting (usually gets set to 0)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float turnRate;

private:

	UFUNCTION()
	void Turn();

	UFUNCTION()
	void BoostSpeed();

	UFUNCTION()
	void DeboostSpeed();

	UFUNCTION()
	void Halt();

	UFUNCTION()
	void UnHalt();

	UFUNCTION()
	float CalculateSpeed(); //uses baseSpeed, speed modifiers and properties of segments attached to give final movement speed

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
