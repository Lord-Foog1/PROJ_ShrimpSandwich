// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellUpgradeComponent.h"
#include "UpgradeManager.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USpellUpgradeComponent::USpellUpgradeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USpellUpgradeComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(USpellUpgradeComponent, SpellDamageModifier);
	DOREPLIFETIME(USpellUpgradeComponent, SpellSpeedModifier);
	DOREPLIFETIME(USpellUpgradeComponent, SpellLifetimeModifier);
}


// Called when the game starts
void USpellUpgradeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USpellUpgradeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USpellUpgradeComponent::ApplyUpgrades(EUpgradeModifierType StatToUpgrade, float DeltaValue)
{
	if (!GetOwner()->GetOwner()->HasAuthority())
	{
		return;
	}
	
	float* TargetModifier = nullptr;
	
	switch (StatToUpgrade)
	{
	case EUpgradeModifierType::SpellDamage:
		TargetModifier = &SpellDamageModifier;
		break;
	case EUpgradeModifierType::SpellSpeed:
		TargetModifier = &SpellSpeedModifier;
		break;
	case EUpgradeModifierType::SpellLifetime:
		TargetModifier = &SpellLifetimeModifier;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Stat Chosen Does Not Exist On '%s'"), *GetNameSafe(GetOwner()));
		break;
	}
	
	if (TargetModifier)
	{
		*TargetModifier = FMath::Max(0.05f, *TargetModifier + DeltaValue);
	}
}

