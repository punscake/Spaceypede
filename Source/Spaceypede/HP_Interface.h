// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HP_Interface.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEYPEDE_API UHP_Interface : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHP_Interface();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIncrementHealthDelegate, float, healthIncrement);
	UPROPERTY(BlueprintAssignable, Category = "HP")
		FOnIncrementHealthDelegate OnIncrementHealthDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHelthBelowZero);
	UPROPERTY(BlueprintAssignable, Category = "HP")
		FOnHelthBelowZero OnHelthBelowZero;

	UPROPERTY()
		float currentHP;

	UPROPERTY()
		float maxHP;

	UPROPERTY(EditAnywhere)
		float maxIncomingDPS; //used for calculating invuln frames

	UPROPERTY()
		float currentInvulnerabilityTimer;

	UPROPERTY(EditAnywhere)
		bool bUseInvulnerability;

	UPROPERTY(EditAnywhere)
		bool bCanBeHealed;

	UPROPERTY()
		float invincibilityGainTimeSeconds;

	UFUNCTION()
		void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
		float CalculateTrueDamage(float damage);

	UFUNCTION()
		bool IsInvincible();

	UFUNCTION()
		void ProcInvincibility(float damage);


public:
	UFUNCTION()
	float DamageInstancesToDeath(float damageInstance); //how many damage instances are needed to set hp to 0

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};