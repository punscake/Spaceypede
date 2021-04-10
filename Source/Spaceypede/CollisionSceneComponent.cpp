// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionSceneComponent.h"


// Sets default values for this component's properties
UCollisionSceneComponent::UCollisionSceneComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	NumOfPiercesAvailable = 0;
}


void UCollisionSceneComponent::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	//attempts to find child CollisionSceneComponent for reaction information
	UCollisionSceneComponent* otherCollisionComponent = UsefulFunctions::FindFirstChildMatchingClassType<UCollisionSceneComponent>(OtherComponent); //defined in header
	if (otherCollisionComponent == nullptr) {
		return;
	}
	
	//TODO ethereal mode

	//ignore objects already affected (pierced for example)
	if (CheckIfAlreadyAffected(otherCollisionComponent)) {
		return;
	}

	//check component types and corresponding reactions
	FStructReactionCasesDeclaration otherComponentsReactionCases = otherCollisionComponent->GetStructReactionCases();
	EnumCollisionComponentTypeDeclaration otherComponentType = otherComponentsReactionCases.EnumCollisionComponentType;

	//ignores normal collision logic, calls custom function to decide what happens now
	if (StructReactionCases.EnumCollisionComponentType == CallCustom) {
		if (StructCustomCollide.CustomCollide) {
			StructCustomCollide.CustomCollide(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, Hit, this, otherCollisionComponent);
		}
		return;
	}
	else if (otherComponentType == CallCustom) {
		return;
	}
	
	//get preferred infliction and reaction for colliding actors
	EnumCollisionPossibleReactionsDeclaration ownInfliction; //own infliction to other collider	
	if (otherComponentType == Soft) {
		ownInfliction = StructReactionCases.InflictsOnSoft;
	}
	else if (otherComponentType == Tough) {
		ownInfliction = StructReactionCases.InflictsOnTough;
	}

	EnumCollisionPossibleReactionsDeclaration otherReaction; //other collider's reaction on the type of this collider
	if (StructReactionCases.EnumCollisionComponentType == Soft) {
		otherReaction = otherComponentsReactionCases.ReactsToSoft;
	}
	else if (StructReactionCases.EnumCollisionComponentType == Tough) {
		otherReaction = otherComponentsReactionCases.ReactsToTough;
	}

	//calculate reaction
	EnumCollisionPossibleReactionsDeclaration FinalReaction = CalculateReaction(ownInfliction, otherReaction);

	//process reaction
	ProcessReaction(FinalReaction, OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, Hit, this, otherCollisionComponent);
}

bool UCollisionSceneComponent::CheckIfAlreadyAffected(UCollisionSceneComponent* componentToCheck)
{
	for (UCollisionSceneComponent* i : AlreadyAffectedComponents) {
		if (componentToCheck == i) {
			return true;
		}
	}
	return false;
}

bool UCollisionSceneComponent::AttemptAddingToAlreadyAffected(UCollisionSceneComponent* componentToAdd)
{
	for (UCollisionSceneComponent* i : AlreadyAffectedComponents) {
		if (i == nullptr) {
			i = componentToAdd;
			return true;
		}
	}
	return false;
}

bool UCollisionSceneComponent::SetNumOfPiercesAvailable(unsigned int value)
{
	if (HasBegunPlay()) {
		return false;
	}
	NumOfPiercesAvailable = value;
	return true;
}

EnumCollisionPossibleReactionsDeclaration UCollisionSceneComponent::CalculateReaction(EnumCollisionPossibleReactionsDeclaration ownInfliction, EnumCollisionPossibleReactionsDeclaration otherReaction)
{
	if (otherReaction == Ignore) {
		return Ignore;
	}
	else if (ownInfliction == Destroy) {
		return Destroy;
	}
	else if (ownInfliction != Inherit || otherReaction != Inherit) { //if at least one reaction is not Inherit, otherwise nothing happens
		if (ownInfliction == Inherit) {
			return otherReaction;
		}
		else if (otherReaction == Inherit) {
			return ownInfliction;
		}
		else if (ownInfliction == otherReaction) {
			return ownInfliction; //one or the other, since they are the same
		}
	}
	return Inherit;
}

void UCollisionSceneComponent::ProcessReaction(EnumCollisionPossibleReactionsDeclaration reaction, UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& Hit, UCollisionSceneComponent* thisCollisionComponent, UCollisionSceneComponent* otherCollisionComponent)
{
	switch (reaction)
	{
	case Inherit:
		break;
	case FlatDecrementHP:
		if (StructCustomDamage.CustomDamage) {
			StructCustomDamage.CustomDamage(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, Hit, thisCollisionComponent, otherCollisionComponent);
		}
		else {
			FDamageEvent dmgEvent = FDamageEvent();
			AController* controller = UsefulFunctions::AttemptFindingAController(AActorForOperations);
			OtherActor->TakeDamage(FlatDecrementHP, dmgEvent, controller, AActorForOperations);
		}
		break;
	case Destroy:
		if (StructCustomDestroy.CustomDestroy) {
			StructCustomDestroy.CustomDestroy(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, Hit, thisCollisionComponent, otherCollisionComponent);
		}
		else {
			DestroyActorUpdatingStats(otherCollisionComponent->GetAActorForOperations(), thisCollisionComponent->GetAActorForOperations());
		}
		break;
	case HPforHP:
		HPforHPfunction(thisCollisionComponent->GetAActorForOperations(), thisCollisionComponent->GetFlatHPIncrementAmount(),
			otherCollisionComponent->GetAActorForOperations(), otherCollisionComponent->GetFlatHPIncrementAmount());
		break;
	case Ignore:
		break;
	}
}

void UCollisionSceneComponent::HPforHPfunction(AActor* firstActor, float firstActorDamageInstance, AActor* secondActor, float secondActorDamageInstance)
{
	UHP_Interface* firstHPinterface = Cast<UHP_Interface>(firstActor->GetComponentByClass(UHP_Interface::StaticClass()));
	UHP_Interface* secondHPinterface = Cast<UHP_Interface>(secondActor->GetComponentByClass(UHP_Interface::StaticClass()));
	//count how many instances of damage from the opposing actor it would take for each actor to die
	float firstActorDamageInstanceCount = firstHPinterface->DamageInstancesToDeath(secondActorDamageInstance);
	float secondActorDamageInstanceCount = secondHPinterface->DamageInstancesToDeath(firstActorDamageInstance);
	
	if (firstActorDamageInstanceCount > secondActorDamageInstanceCount) { //if second actor to be destroyer
		if (firstActorDamageInstanceCount - secondActorDamageInstanceCount > MUTUALDESTRUCTIONDIFFERENCETHRESHOLD) {
			//destroy
			DestroyActorUpdatingStats(secondActor, firstActor);
			//damage first
			FDamageEvent dmgEvent = FDamageEvent();
			AController* controller = UsefulFunctions::AttemptFindingAController(secondActor);
			firstActor->TakeDamage(secondActorDamageInstance * secondActorDamageInstanceCount, dmgEvent, controller, secondActor);
		}
	}
	else if (secondActorDamageInstanceCount > firstActorDamageInstanceCount) { //if first actor to be destroyer
		if (firstActorDamageInstanceCount - secondActorDamageInstanceCount > MUTUALDESTRUCTIONDIFFERENCETHRESHOLD) {
			//destroy
			DestroyActorUpdatingStats(firstActor, secondActor);
			//damage second
			FDamageEvent dmgEvent = FDamageEvent();
			AController* controller = UsefulFunctions::AttemptFindingAController(firstActor);
			secondActor->TakeDamage(firstActorDamageInstance * firstActorDamageInstanceCount, dmgEvent, controller, firstActor);
		}
	}
	else {
		DestroyActorUpdatingStats(secondActor, firstActor);
		DestroyActorUpdatingStats(firstActor, secondActor);
	}

}

void UCollisionSceneComponent::DestroyActorUpdatingStats(AActor* toDestroy, AActor* destroyer)
{
	toDestroy->Destroy();//TODO
}

AActor* UCollisionSceneComponent::GetAActorForOperations()
{
	return AActorForOperations;
}

void UCollisionSceneComponent::SetAActorForOperations(AActor* actor)
{
	AActorForOperations = actor;
}

FStructReactionCasesDeclaration UCollisionSceneComponent::GetStructReactionCases()
{
	return StructReactionCases;
}

void UCollisionSceneComponent::SetStructReactionCases(FStructReactionCasesDeclaration newValue)
{
	StructReactionCases = newValue;
}

void UCollisionSceneComponent::SetFlatHPIncrementAmount(float HPtoIncrement)
{
	FlatHPIncrementAmount = HPtoIncrement;
}

float UCollisionSceneComponent::GetFlatHPIncrementAmount()
{
	return FlatHPIncrementAmount;
}


// Called when the game starts
void UCollisionSceneComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* ownerA = GetOwner();
	if (ownerA && ownerA->IsA<UPrimitiveComponent>()) {
		UPrimitiveComponent* ownerPrimitiveComponent = Cast<UPrimitiveComponent>(ownerA);

		ownerPrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &UCollisionSceneComponent::OnCollision);
	}
	
	AlreadyAffectedComponents.Init(nullptr, NumOfPiercesAvailable);
}


// Called every frame
void UCollisionSceneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

