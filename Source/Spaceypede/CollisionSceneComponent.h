// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HP_Interface.h"
#include "UsefulFunctions.h"
#include "CollisionSceneComponent.generated.h"

UENUM()
enum EnumCollisionComponentTypeDeclaration {
	Soft      UMETA(DisplayName = "Soft"),
	Tough     UMETA(DisplayName = "Tough"),
	CallCustom   UMETA(DisplayName = "CallCustom"),
}; //Tough-on-Tough means one of them has to die, Soft has piercing count and then dies, CallCustom causes no interaction with other collisions, but instead calls own event

UENUM()
enum EnumCollisionPossibleReactionsDeclaration { //used by StructReactionCases
	Inherit     UMETA(DisplayName = "Inherit"), //agrees to use other colliders reaction, otherwise they have to be the same for collided actors, if both "Inherit" then no reaction
	FlatDecrementHP      UMETA(DisplayName = "SetDamage"), //negative decrement = healing
	Destroy   UMETA(DisplayName = "Destroy"), //actor must die, overrides everything except "Ignore"
	HPforHP   UMETA(DisplayName = "HPforHP"), //one has to die, other takes proportionate dmg
	Ignore   UMETA(DisplayName = "Ignore"), //refuses to react, complete override
}; //for logic see .cpp::OnCollision()

USTRUCT()
struct FStructReactionCasesDeclaration {
	GENERATED_BODY()
	//type
	EnumCollisionComponentTypeDeclaration EnumCollisionComponentType;
	//inflicts:
	EnumCollisionPossibleReactionsDeclaration InflictsOnSoft;
	EnumCollisionPossibleReactionsDeclaration InflictsOnTough;
	//reacts:
	EnumCollisionPossibleReactionsDeclaration ReactsToSoft;
	EnumCollisionPossibleReactionsDeclaration ReactsToTough;
};

//hold custom function references, see ProcessReaction() definition
USTRUCT()
struct FStructCustomCollideContainer {
	GENERATED_BODY()
		void (*CustomCollide)(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit,
			UCollisionSceneComponent* thisCollisionComponent, UCollisionSceneComponent* otherCollisionComponent);//custom behavior if != nullptr; always redirects normal collision information + both CollisionSceneComponents
};

USTRUCT()
struct FStructCustomDamageContainer {
	GENERATED_BODY()
		void (*CustomDamage)(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit,
			UCollisionSceneComponent* thisCollisionComponent, UCollisionSceneComponent* otherCollisionComponent);//custom behavior if != nullptr; when reaction = Damage; redirects normal collision information + both CollisionSceneComponents
};

USTRUCT()
struct FStructCustomDestroyContainer {
	GENERATED_BODY()
		void (*CustomDestroy)(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit,
			UCollisionSceneComponent* thisCollisionComponent, UCollisionSceneComponent* otherCollisionComponent);//custom behavior if != nullptr; when reaction = Destroy; redirects normal collision information + both CollisionSceneComponents
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACEYPEDE_API UCollisionSceneComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
		FStructReactionCasesDeclaration StructReactionCases;

	//damage/heal value
	UPROPERTY()
		float FlatHPIncrementAmount = 0.f;

		const float MUTUALDESTRUCTIONDIFFERENCETHRESHOLD = 0.1f; //HPforHP mode ONLY

	UPROPERTY()
		AActor* AActorForOperations;

	UPROPERTY(EditAnywhere)
		unsigned int NumOfPiercesAvailable;

	UPROPERTY(VisibleAnywhere)
		TArray<UCollisionSceneComponent*> AlreadyAffectedComponents;

	UFUNCTION()
		bool CheckIfAlreadyAffected(UCollisionSceneComponent* componentToCheck); //checks if this actor is in AlreadyAffectedComponents

	UFUNCTION()
		bool AttemptAddingToAlreadyAffected(UCollisionSceneComponent* componentToAdd); //if there is space in the array it adds the component as "affected" and returns true

	UFUNCTION()
		bool SetNumOfPiercesAvailable(unsigned int value); //only callable while HasBegunPlay() is false, returns true if successfully sets value

	UFUNCTION()
		EnumCollisionPossibleReactionsDeclaration CalculateReaction(EnumCollisionPossibleReactionsDeclaration ownInfliction, EnumCollisionPossibleReactionsDeclaration otherReaction);

	UFUNCTION()
		void ProcessReaction(EnumCollisionPossibleReactionsDeclaration reaction, UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& Hit, UCollisionSceneComponent* thisCollisionComponent, UCollisionSceneComponent* otherCollisionComponent);

	UFUNCTION()
		void HPforHPfunction(AActor* firstActor, float firstActorDamageInstance, AActor* secondActor, float secondActorDamageInstance); //one gets destroyed, other takes proportional dmg; both must have HP_Interfaces, if more then 1 present (why?) first one is chosen

	UFUNCTION()
		void DestroyActorUpdatingStats(AActor* toDestroy, AActor* destroyer); //destroys actor, notifies gamemode about the destroyer responsible

public:	
	// Sets default values for this component's properties
	UCollisionSceneComponent();

	//Redirected from collision mesh
	UFUNCTION()
		void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	
	//see ProcessReaction function def
	UPROPERTY()
		FStructCustomCollideContainer StructCustomCollide; // holds custom behavior function

	UPROPERTY()
		FStructCustomDamageContainer StructCustomDamage; // holds custom behavior function
	UPROPERTY()
		FStructCustomDestroyContainer StructCustomDestroy; // holds custom behavior function 

	/*
		void SetCustomCollide(void (*CustomCollide)(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit, UCollisionSceneComponent* thisCollisionComponent, UCollisionSceneComponent* otherCollisionComponent));
	
	
		void SetCustomDamage(void (*CustomDamage)(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit, UCollisionSceneComponent* thisCollisionComponent, UCollisionSceneComponent* otherCollisionComponent));

	
		void SetCustomDestroy(void (*CustomDestroy)(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit, UCollisionSceneComponent* thisCollisionComponent, UCollisionSceneComponent* otherCollisionComponent));
	*/

	//getters/setters
	UFUNCTION()
		AActor* GetAActorForOperations();

	UFUNCTION()
		void SetAActorForOperations(AActor* actor);

	UFUNCTION()
	FStructReactionCasesDeclaration GetStructReactionCases();

	UFUNCTION()
		void SetStructReactionCases(FStructReactionCasesDeclaration newValue);

	UFUNCTION()
		void SetFlatHPIncrementAmount(float HPtoIncrement); //negative means healing, positive means damage

	UFUNCTION()
		float GetFlatHPIncrementAmount();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
