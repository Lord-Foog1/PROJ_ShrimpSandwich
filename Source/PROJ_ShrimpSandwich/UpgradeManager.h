// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UpgradeManager.generated.h"

class AMyCharacter;

UENUM(BlueprintType)
enum class EUpgradeModifierType : uint8
{
	SpellCooldown        UMETA(DisplayName = "Spell Cooldown"),
	MeleeCooldown        UMETA(DisplayName = "Melee Cooldown"),
	ChargeUpTime         UMETA(DisplayName = "Charge Up Time"),
	ManaRegenCooldown    UMETA(DisplayName = "Mana Regen Cooldown"),
	SpellDamage          UMETA(DisplayName = "Spell Damage"),
	MeleeDamage          UMETA(DisplayName = "Melee Damage"),
	ManaCost             UMETA(DisplayName = "Mana Cost"),
	ChargeShotCost       UMETA(DisplayName = "Charge Shot Cost"),
	MaxMana              UMETA(DisplayName = "Max Mana"),
	ManaRegen            UMETA(DisplayName = "Mana Regen"),
	MaxHealth            UMETA(DisplayName = "Max Health")
};

UCLASS()
class PROJ_SHRIMPSANDWICH_API UUpgradeManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	void UpgradeStat(EUpgradeModifierType StatToUpgrade, float DeltaValue, AMyCharacter* TargetCharacter = nullptr);
	
private:	
};
