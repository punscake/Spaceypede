// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelPopulator.h"

bool ULevelPopulator::InitProperties(float newDifficultyScaleMultiplier, unsigned int newSeed, unsigned int newLevelNumber)
{
	if (bAlreadyInitialized) {
		return false;
	}
	difficultyScaleMultiplier = newDifficultyScaleMultiplier;
	seed = newSeed;
	levelNumber = newLevelNumber;
	bAlreadyInitialized = true;

	return true;
}

void ULevelPopulator::SpawnStructures(FStructureAndLocationArray StructureAndLocationArray)
{
}



