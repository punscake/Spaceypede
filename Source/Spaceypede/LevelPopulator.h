// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelPopulationStructs.h"

//#include "LevelPopulator.generated.h"


class LevelPopulator
{
	
private:

	const float DIFFICULTYSCALINGPERMETER = 0.001;

	UPROPERTY()
		float difficultyScaleMultiplier = 0;

	UPROPERTY()
		int seed = 0;

	UPROPERTY()
		int levelNumber = 0;

	UPROPERTY()
		bool bAlreadyInitialized = false;

public:
	UFUNCTION()
		bool InitProperties(float newDifficultyScaleMultiplier, unsigned int newSeed, unsigned int newLevelNumber); //returns false if already initialized
	/*
	UFUNCTION()
		float GetDifficultyAtDistance(float distanceFromCenter);

	UFUNCTION()
		float GetDistanceAtDifficulty(float difficulty);

	UFUNCTION()
		float GetDifficultyAtLocation(FVector location);
		*/
		virtual void SpawnStructures(FStructureAndLocationArray StructureAndLocationArray); //structure generation DEPENDS on this, overriding may break the normal game flow

		virtual void SpawnRandomEnemies (/*enemy array, densityPerSquareM, radius*/) = 0;  //needs to populate the level with enemies with corresponding restrictions and difficulty, shouldn't overlap with other structures

		virtual void SpawnRandomDebris (/*debris array, densityPerSquareM, radius*/) = 0; //needs to populate the level with miselaneous actors with corresponding restrictions and difficulty, shouldn't overlap with other structures and enemies

	
};