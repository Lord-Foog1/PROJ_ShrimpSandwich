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
	SpellDamage          UMETA(DisplayName = "Spell Damage"),
	SpellCost            UMETA(DisplayName = "Spell Cost"),
	SpellSpeed           UMETA(DisplayName = "Spell Speed"),
	SpellLifetime        UMETA(DisplayName = "Spell Lifetime"),
	MeleeCooldown        UMETA(DisplayName = "Melee Cooldown"),
	MeleeDamage          UMETA(DisplayName = "Melee Damage"),
	MeleeRange           UMETA(DisplayName = "Melee Range"),
	ChargeUpTime         UMETA(DisplayName = "Charge Up Time"),
	ChargeShotCost       UMETA(DisplayName = "Charge Shot Cost"),
	ChargeDamage         UMETA(DisplayName = "Charge Damage"),
	ChargeCooldown       UMETA(DisplayName = "Charge Cooldown"),
	ManaRegenCooldown    UMETA(DisplayName = "Mana Regen Cooldown"),
	ManaRegen            UMETA(DisplayName = "Mana Regen"),
	MaxMana              UMETA(DisplayName = "Max Mana"),
	MaxHealth            UMETA(DisplayName = "Max Health")
};

UCLASS()
class PROJ_SHRIMPSANDWICH_API UUpgradeManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	/**
	 * 
	 * Upgrades the stats of the selected player
	 * 
	 * @param StatToUpgrade Which stat to upgrade
	 * @param DeltaValue How much the multiplier should increase by (ex. 0.1 == 10%)
	 * @param TargetCharacter The character that is to be upgrade
	*/
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	void UpgradePlayerStat(EUpgradeModifierType StatToUpgrade, float DeltaValue, AMyCharacter* TargetCharacter = nullptr);
	
	/**
	 * Upgrades the stats of the selected spell
	 * 
	 * @param StatToUpgrade Which stat to be upgraded
	 * @param DeltaValue How much the multiplier should increase by (ex. 0.1 == 10%)
	 * @param TargetSpell The spell that is to be upgrade
	 */
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	void UpgradeSpellStat(EUpgradeModifierType StatToUpgrade, float DeltaValue, ASpell* TargetSpell = nullptr);
};
