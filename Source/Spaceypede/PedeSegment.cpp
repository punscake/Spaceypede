// Fill out your copyright notice in the Description page of Project Settings.

#define _USE_MATH_DEFINES

#include "PedeSegment.h"
#include <cmath>
#include <string>


// Sets default values
APedeSegment::APedeSegment()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RotatingComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RotatingComponent"));
	RotatingComponent->SetupAttachment(RootComponent);

	MainMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMeshComponent"));
	MainMeshComponent->SetupAttachment(RotatingComponent);

	CollisionNose = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionNose"));
	CollisionNose->SetupAttachment(RotatingComponent);
	CollisionNose->InitSphereRadius(40.0f);
	CollisionNose->SetCollisionProfileName(TEXT("CollisionNoseHitbox"));
	CollisionNose->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
	//CollisionNose->AddLocalOffset(FVector(50.0f, 0.0f, 0.0f));

	CollisionButt = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionButt"));
	CollisionButt->SetupAttachment(RotatingComponent);
	CollisionButt->InitSphereRadius(40.0f);
	CollisionButt->SetCollisionProfileName(TEXT("CollisionButtHitbox"));
	CollisionButt->SetRelativeLocation(FVector(-50.0f, 0.0f, 0.0f));

	CollisionContactDamage = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionContactDamage"));
	CollisionContactDamage->SetupAttachment(RotatingComponent);
	CollisionContactDamage->InitSphereRadius(40.0f);
	CollisionContactDamage->SetCollisionProfileName(TEXT("CollisionContactDamageHitbox"));

	TopSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TopSpringArm"));
	TopSpringArm->SetupAttachment(RootComponent);
	TopSpringArm->TargetArmLength = 700.0f;
	TopSpringArm->bInheritYaw = false;
	TopSpringArm->bDoCollisionTest = false;
	TopSpringArm->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	TopViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopViewCamera"));
	TopViewCamera->SetupAttachment(TopSpringArm);
	TopViewCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	

	

	

	bIsPossessed = false;
	bIsRogue = true;
	baseSpeed = 300.0;
	speedBoostModifier = 1.0;
	speedHaltModifier = 0.0;
	turnMinCircleRadius = 500.0;

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

	if (true || bIsPossessed) {
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
	//InputComponent->BindAction("SlowDown", IE_Released, this, &APedeSegment::UnHalt);

	InputComponent->BindAxis("TurnBinary", this, &APedeSegment::SetTurnDirection);


	//InputComponent->BindAction("Shoot", IE_Pressed, this, &APedeSegment::);
}

void APedeSegment::SetTurnDirection(float value) {
	turnDirection = value; //WARNING: setting value to something other that -1.0, 0.0, or 1.0 may result in logic error in moveAndLeaveTrail if statement
}

void APedeSegment::BoostSpeed() {
	speedBoostModifier = 2.0;
	UnHalt();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Boost pressed")));
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
	float distanceToTravel = speed * DeltaTime; //distance along the circumference of a circle with radius turnMinCircleRadius to the wanted point, or straight up distance forward
	float currentTurnDirection = turnDirection; //snapshots the turnDirection for furher calculations
	
	// checks if turning or going straight, turns RotatingComponent if turning; NOTE: threshholds are arbitrary, possible cases are -1.0, 0.0, 1.0
	if (currentTurnDirection > 0.5 || currentTurnDirection < -0.5) {

		float turnAngle = CalculateTurnAngle(distanceToTravel) * currentTurnDirection; //angle counted from a line tangent to the circle
		distanceToTravel = CalculateDistanceCutThroughCircle(fabs(turnAngle)); //distance needed to reach wanted point on the circle by cutting at angle turnAngle

		//TODO calculation of turn angle for directional movement

		RotatePawnRight(turnAngle);
		BlinkPawnForward(distanceToTravel);
		RotatePawnRight(turnAngle); //rotation is done twice to align the view with a tangent to the circle at that point
	}
	else {
		BlinkPawnForward(distanceToTravel);
	}
	

	//report to Leads



	/*turn based on turn rate
		* blink forward based on speed
		*
		* leave trail for connected segment
		* report distance traveled to connected segment
		*/
}


void APedeSegment::BlinkPawnForward(float distance) {
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Distance = %f"), distanceToTravel));
	//move forward
	FVector moveVector = RotatingComponent->GetRelativeRotation().Vector() * distance;
	FVector newLocation = GetActorLocation() + moveVector;
	SetActorLocation(newLocation);
}


void APedeSegment::RotatePawnRight(float angleInRadiants) {
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Turn direction = %f, Turn angle = %f, distance = %f"), currentTurnDirection, turnAngle, distanceTraveledInCircle));
		//turn
	FRotator newRotation = RotatingComponent->GetRelativeRotation();
	newRotation.Yaw += angleInRadiants * 180.0;
	RotatingComponent->SetRelativeRotation(newRotation);
}

float APedeSegment::CalculateSpeed() {
	float resultingSpeed = baseSpeed;
	return baseSpeed * speedBoostModifier * speedHaltModifier; //TODO other modifiers
}

float APedeSegment::CalculateTurnAngle(float distanceTraveledInCircle) {
	float circleSegmentAngle = distanceTraveledInCircle / turnMinCircleRadius; //angle in Pi, theta/2pi = distance/r2pi
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("circum = %f, angle = %f"), halfCircumference, circleSegmentAngle));
	return circleSegmentAngle / 2; //angle in Pi
}

float APedeSegment::CalculateDistanceCutThroughCircle(float turnAngle) {
	float sinOfTurnAngle = sin(turnAngle);
	return sinOfTurnAngle * turnMinCircleRadius * 2;
}

