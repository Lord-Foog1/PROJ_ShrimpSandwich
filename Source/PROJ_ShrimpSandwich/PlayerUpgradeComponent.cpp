// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUpgradeComponent.h"
#include "UpgradeManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPlayerUpgradeComponent::UPlayerUpgradeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	
	// ...
}

void UPlayerUpgradeComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UPlayerUpgradeComponent, SpellCostModifier);
	DOREPLIFETIME(UPlayerUpgradeComponent, SpellCooldownModifier);
	
	DOREPLIFETIME(UPlayerUpgradeComponent, MaxManaModifier);
	DOREPLIFETIME(UPlayerUpgradeComponent, ManaRegenModifier);
	DOREPLIFETIME(UPlayerUpgradeComponent, ManaRegenIntervalModifier);
	
	DOREPLIFETIME(UPlayerUpgradeComponent, ChargedDamageModifier);
	DOREPLIFETIME(UPlayerUpgradeComponent, ChargedCostModifier);
	DOREPLIFETIME(UPlayerUpgradeComponent, ChargedCooldownModifier);
	DOREPLIFETIME(UPlayerUpgradeComponent, ChargeTimeModifier);
	
	DOREPLIFETIME(UPlayerUpgradeComponent, MeleeDamageModifier);
	DOREPLIFETIME(UPlayerUpgradeComponent, MeleeCooldownModifier);
	DOREPLIFETIME(UPlayerUpgradeComponent, MeleeRangeModifier);
	
	DOREPLIFETIME(UPlayerUpgradeComponent, MaxHealthModifier);
}


// Called when the game starts
void UPlayerUpgradeComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UPlayerUpgradeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerUpgradeComponent::ApplyUpgrades(EUpgradeModifierType StatToUpgrade, float DeltaValue)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	float* TargetModifier = nullptr;

	switch (StatToUpgrade)
	{
	case EUpgradeModifierType::SpellCooldown:
		TargetModifier = &SpellCooldownModifier;
		break;
	case EUpgradeModifierType::SpellCost:
		TargetModifier = &SpellCostModifier;
		break;
	case EUpgradeModifierType::MeleeCooldown:
		TargetModifier = &MeleeCooldownModifier;
		break;
	case EUpgradeModifierType::MeleeDamage:
		TargetModifier = &MeleeDamageModifier;
		break;
	case EUpgradeModifierType::MeleeRange:
		TargetModifier = &MeleeRangeModifier;
		break;
	case EUpgradeModifierType::ChargeUpTime:
		TargetModifier = &ChargeTimeModifier;
		break;
	case EUpgradeModifierType::ChargeShotCost:
		TargetModifier = &ChargedCostModifier;
		break;
	case EUpgradeModifierType::ChargeDamage:
		TargetModifier = &ChargedDamageModifier;
		break;
	case EUpgradeModifierType::ChargeCooldown:
		TargetModifier = &ChargedCooldownModifier;
		break;
	case EUpgradeModifierType::ManaRegenCooldown:
		TargetModifier = &ManaRegenIntervalModifier;
		break;
	case EUpgradeModifierType::ManaRegen:
		TargetModifier = &ManaRegenModifier;
		break;
	case EUpgradeModifierType::MaxMana:
		TargetModifier = &MaxManaModifier;
		break;
	case EUpgradeModifierType::MaxHealth:
		TargetModifier = &MaxHealthModifier;
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

