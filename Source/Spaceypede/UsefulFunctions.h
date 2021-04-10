// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class SPACEYPEDE_API UsefulFunctions
{
public:
	UsefulFunctions();
	~UsefulFunctions();

	template <class Type>
	 static Type* FindFirstChildMatchingClassType(USceneComponent* sceneComponent) {
		TArray<USceneComponent*> children;
		sceneComponent->GetChildrenComponents(false, children);
		USceneComponent** toBeCast = children.FindByPredicate([](const USceneComponent* item)
			{
				return item->IsA<Type>();
			}
		);
		if (!*toBeCast) { //if could not find anything stops the function
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Couldn't find component")));
			return nullptr;
		}
		Type* otherComponent = Cast<Type>(*toBeCast);
		if (!otherComponent) { //if cast failed stops the function
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Couldn't cast component")));
			return nullptr;
		}
		return otherComponent;
	}

	 static AController* AttemptFindingAController(AActor* actor) {
		 AController* controller = nullptr;
		 if (actor->IsA<APawn>()) {

			 controller = Cast<APawn>(actor)->GetController();
		 }
		 return controller;
	 }
};
