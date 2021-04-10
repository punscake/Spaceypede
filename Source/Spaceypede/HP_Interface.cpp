// Fill out your copyright notice in the Description page of Project Settings.


#include "HP_Interface.h"

// Sets default values for this component's properties
UHP_Interface::UHP_Interface()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	invincibilityGainTimeSeconds = 0.0;
}


// Called when the game starts
void UHP_Interface::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner) {
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHP_Interface::TakeDamage);
	}
	
}


void UHP_Interface::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	float TrueDecrement = CalculateTrueDamage(Damage);
	if (TrueDecrement < 0) {	//if HEALING
		float toMaxHP = maxHP - currentHP;
		TrueDecrement *= -1;
		if (TrueDecrement > toMaxHP) {
			currentHP = maxHP;
			OnIncrementHealthDelegate.Broadcast(toMaxHP);
		}
		else {
			currentHP += TrueDecrement;
			OnIncrementHealthDelegate.Broadcast(TrueDecrement);
		}
	}
	else if (!IsInvincible()) {
		if (currentHP > TrueDecrement) {
			OnIncrementHealthDelegate.Broadcast(TrueDecrement * -1);
			currentHP -= TrueDecrement;
			ProcInvincibility(TrueDecrement);
		}
		else {
			OnIncrementHealthDelegate.Broadcast(currentHP * -1);
			currentHP = 0.0;
			OnHelthBelowZero.Broadcast();	//death
		}
	}
}

float UHP_Interface::CalculateTrueDamage(float damage)
{
	return damage;
}

bool UHP_Interface::IsInvincible()
{
	float timeSinceLast = GetWorld()->GetTimeSeconds() - invincibilityGainTimeSeconds;
	if (timeSinceLast > currentInvulnerabilityTimer) {
		currentInvulnerabilityTimer = 0.f;
		return true;
	}
	return false;
}

void UHP_Interface::ProcInvincibility(float damage)
{
	float percentageOfMaxDPS = damage / maxIncomingDPS;
	currentInvulnerabilityTimer = percentageOfMaxDPS;
	invincibilityGainTimeSeconds = GetWorld()->GetTimeSeconds();
}

float UHP_Interface::DamageInstancesToDeath(float damageInstance)
{
	if (damageInstance <= 0.f) {
		return 999999.f; //TODO make this not trash
	}
	float TrueDamage = CalculateTrueDamage(damageInstance);
	return currentHP / TrueDamage;
}

// Called every frame
void UHP_Interface::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

