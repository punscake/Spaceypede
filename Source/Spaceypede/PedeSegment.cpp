// Fill out your copyright notice in the Description page of Project Settings.

#define _USE_MATH_DEFINES

#include "PedeSegment.h"
#include <cmath>


// Sets default values
APedeSegment::APedeSegment()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	MainMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMeshComponent"));

	CollisionNose = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionNose"));
	CollisionNose->InitSphereRadius(40.0f);
	CollisionNose->SetCollisionProfileName(TEXT("CollisionNoseHitbox"));
	CollisionNose->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));

	CollisionButt = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionButt"));
	CollisionButt->InitSphereRadius(40.0f);
	CollisionButt->SetCollisionProfileName(TEXT("CollisionButtHitbox"));
	CollisionButt->SetRelativeLocation(FVector(-50.0f, 0.0f, 0.0f));

	CollisionContactDamage = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionContactDamage"));
	CollisionContactDamage->InitSphereRadius(40.0f);
	CollisionContactDamage->SetCollisionProfileName(TEXT("CollisionContactDamageHitbox"));

	

	isPossessed = false;
	isRogue = true;
	baseSpeed = 10.0;
	speedBoostModifier = 1.0;
	speedHaltModifier = 1.0;

	Leads = nullptr;
	LedBy = nullptr;
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

	if (isPossessed) {
		moveAndLeaveTrail(DeltaTime);
	}
	/*
	* calculate speed
	* if possesed
	*	turn based on turn rate
	*	blink forward based on speed
	*
	*	leave trail for connected segment
	*	report distance traveled to connected segment
	* else if has trail
	*	get distance traveled, continue if more than 0
	*	get last trail from stack
	*	calculate distance to trail
	*
	*	if distance traveled < distance to trail
	*		move distance traveled towards trail
	*		put trail back
	*	else if distance traveled = distance to trail
	*		move to trail
	*	else
	*		distanc traveled -= distance to trail
	*		move to trail
	*		repeat the if statement
	*
	*	leave trail for connected segment
	*	report distance traveled to connected segment
	*
	* else
	*	physics for rogue objects
	*/
}

// Called to bind functionality to input
void APedeSegment::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("SpeedUp", IE_Pressed, this, &APedeSegment::BoostSpeed);
	InputComponent->BindAction("SpeedUp", IE_Released, this, &APedeSegment::DeboostSpeed);
	InputComponent->BindAction("SlowDown", IE_Pressed, this, &APedeSegment::Halt);
	InputComponent->BindAction("SlowDown", IE_Released, this, &APedeSegment::UnHalt);

	InputComponent->BindAxis("TurnBinary", this, &APedeSegment::SetTurnDirection);


	//InputComponent->BindAction("Shoot", IE_Pressed, this, &APedeSegment::);
}

void APedeSegment::SetTurnDirection(float value) {
	turnDirection = value;
}

void APedeSegment::BoostSpeed() {
	speedBoostModifier = 2.0;
}

void APedeSegment::DeboostSpeed() {
	speedBoostModifier = 1.0;
}

void APedeSegment::Halt() {
	speedHaltModifier = 0.0;
}

void APedeSegment::UnHalt() {
	speedHaltModifier = 1.0;
}

void APedeSegment::moveAndLeaveTrail(float DeltaTime) {

	float speed = CalculateSpeed();
	float distanceTraveledInCircle = speed * DeltaTime; //distance along the circumference of a circle with radius turnMinCircleRadius to the wanted point
	float currentTurnDirection = turnDirection; //snapshots the turnDirection for furher calculations
	float turnAngle = CalculateTurnAngle(distanceTraveledInCircle) * abs(currentTurnDirection); //angle counted from a line tangent to the circle
	float finalDistance = CalculateDistanceCutThroughCircle(turnAngle); //distance needed to reach wanted point on the circle by cutting at angle turnAngle
	if (currentTurnDirection < 0) {
		turnAngle *= -1; //undoes the abs()
	}
	//TODO calculation of turn angle for directional movement

	//turn
	//blink
	//report to Leads



	/*turn based on turn rate
		* blink forward based on speed
		*
		* leave trail for connected segment
		* report distance traveled to connected segment
		*/
}

float APedeSegment::CalculateSpeed() {
	float resultingSpeed = baseSpeed;
	return baseSpeed * speedBoostModifier * speedHaltModifier; //TODO other modifiers
}

float APedeSegment::CalculateTurnAngle(float distanceTraveledInCircle) {
	float circumference = turnMinCircleRadius * PI * 2;
	float circleSegmentAngle = circumference / distanceTraveledInCircle;
	return circleSegmentAngle / 2;
}

float APedeSegment::CalculateDistanceCutThroughCircle(float turnAngle) {
	float sinOfTurnAngle = sin(turnAngle);
	return sinOfTurnAngle * turnMinCircleRadius * 2;
}

