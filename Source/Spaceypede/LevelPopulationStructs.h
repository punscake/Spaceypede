#pragma once

/*
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
*/
//#include "StructureForGeneration.h"
#include "StructurePlacer.h"

#include "LevelPopulationStructs.generated.h"

USTRUCT()
struct FStructureAndLocation {
	GENERATED_BODY()

		bool bHasValues;
	FVector directionFromCenter;
	FVector pointingTowards;
	class TSubclassOf<UStructurePlacer> * structure;
	//class AStructureForGeneration* structure;

	float atRadius; //negative number means ignored; priority over difficulty
	//or
	float atDifficulty; //negative number means ignored

	bool operator==(const FStructureAndLocation& a) const
	{
		return false;
	}
};

USTRUCT()
struct FStructureAndLocationArray {
	GENERATED_BODY()


	UPROPERTY()
		TArray<FStructureAndLocation> StructureAndLocationArray;

	FStructureAndLocationArray() {
		FStructureAndLocation empty;
		StructureAndLocationArray.Init(empty, 10);
	}
	bool AddStructure(FStructureAndLocation* StructureAndLocation, bool bIfExtistsReplace) //bIfExtistsReplace = true means it replaces StructureAndLocation that has the same AStructureForGeneration* if possible; returns true if structure already existed
	{
		//check if pointer to new structure is already existant and replace that entry if it is
		TSubclassOf<UStructurePlacer> * structureToCheck = StructureAndLocation->structure;
		for (FStructureAndLocation i : StructureAndLocationArray) {
			if (i.structure == structureToCheck) {
				if (bIfExtistsReplace) {
					i = *StructureAndLocation;
				}
				return true;
			}
		}
		StructureAndLocationArray.Add(*StructureAndLocation);
		return false;
	}

	bool RemoveStructure(TSubclassOf<UStructurePlacer> * Structure) //returns true on success
	{
		for (FStructureAndLocation i : StructureAndLocationArray) {
			if (i.structure == Structure) {
				StructureAndLocationArray.Remove(i);
				return true;
			}
		}
		return false;
	}

	FStructureAndLocation* FindStructureAndLocation(TSubclassOf<UStructurePlacer> * Structure) //attempts to FStructureAndLocation by AStructureForGeneration
	{
		for (FStructureAndLocation i : StructureAndLocationArray) {
			if (i.structure == Structure) {
				return &i;
			}
		}
		return nullptr;
	}
};

/*TODO
USTRUCT()
struct FStructureAndLocationArray {
	GENERATED_BODY()


		UPROPERTY()
		TArray<FStructureAndLocation> StructureAndLocationArray;

	FStructureAndLocationArray() {
		FStructureAndLocation empty;
		StructureAndLocationArray.Init(empty, 10);
	}
	bool AddStructure(FStructureAndLocation* StructureAndLocation, bool bIfExtistsReplace) //bIfExtistsReplace = true means it replaces StructureAndLocation that has the same AStructureForGeneration* if possible; returns true if structure already existed
	{
		//check if pointer to new structure is already existant and replace that entry if it is
		AStructureForGeneration* structureToCheck = StructureAndLocation->structure;
		for (FStructureAndLocation i : StructureAndLocationArray) {
			if (i.structure = structureToCheck) {
				if (bIfExtistsReplace) {
					i = *StructureAndLocation;
				}
				return true;
			}
		}
		StructureAndLocationArray.Add(*StructureAndLocation);
		return false;
	}

	bool RemoveStructure(AStructureForGeneration* Structure) //returns true on success
	{
		for (FStructureAndLocation i : StructureAndLocationArray) {
			if (i.structure = Structure) {
				StructureAndLocationArray.Remove(i);
				return true;
			}
		}
		return false;
	}

	FStructureAndLocation* FindStructureAndLocation(AStructureForGeneration* Structure) //attempts to FStructureAndLocation by AStructureForGeneration
	{
		for (FStructureAndLocation i : StructureAndLocationArray) {
			if (i.structure = Structure) {
				return &i;
			}
		}
		return nullptr;
	}
};*/