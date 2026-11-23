// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerUpgradeComponent.generated.h"

enum class EUpgradeModifierType : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_SHRIMPSANDWICH_API UPlayerUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerUpgradeComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void ApplyUpgrades(EUpgradeModifierType StatToUpgrade, float DeltaValue);
	
#pragma region MODIFIERS
	
#pragma region SPELL
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Spell")
	float SpellCostModifier = 1.0f;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Spell")
	float SpellCooldownModifier = 1.0f;
	
#pragma endregion SPELL
	
#pragma region MANA
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Mana")
	float MaxManaModifier = 1.0f;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Mana")
	float ManaRegenModifier = 1.0f;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Mana")
	float ManaRegenIntervalModifier = 1.0f;
	
#pragma endregion MANA
	
#pragma region CHARGED_SPELL
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Charged")
	float ChargedDamageModifier = 1.0f;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Charged")
	float ChargedCostModifier = 1.0f;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Charged")
	float ChargedCooldownModifier = 1.0f;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Charged")
	float ChargeTimeModifier = 1.0f;
	
#pragma endregion CHARGED_SPELL
	
#pragma region MELEE
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Melee")
	float MeleeDamageModifier = 1.0f;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Melee")
	float MeleeCooldownModifier = 1.0f;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Melee")
	float MeleeRangeModifier = 1.0f;
	
#pragma endregion MELEE
	
#pragma region HEALTH
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Modifiers|Health")
	float MaxHealthModifier = 1.0f;
	
#pragma endregion HEALTH	
	
#pragma endregion MODIFIERS	
	
#pragma region BASE_STATS
	
#pragma region SPELL
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	int BaseSpellCost;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseSpellCooldown;
	
#pragma endregion SPELL
	
#pragma region MANA
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	int BaseMaxMana;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	int BaseManaRegen;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseManaRegenInterval;
	
#pragma endregion MANA
	
#pragma region CHARGED_SPELL
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseChargedDamage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	int BaseChargedCost;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseChargedCooldown;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseChargeTime;
	
#pragma endregion CHARGED_SPELL
	
#pragma region MELEE
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseMeleeDamage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseMeleeCooldown;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseMeleeRange;
	
#pragma endregion MELEE
	
#pragma region HEALTH
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats|Mana")
	float BaseMaxHealth;
	
#pragma endregion HEALTH	
	
#pragma endregion BASE_STATS	
};

/**
 * 
 * Stats to Be Upgradeable:
 * 
 * Spell Damage Amount,
 * Spell Cost,
 * Spell Cooldown,
 * Spell Speed,
 * Spell Lifetime
 * --------------------------
 * Max Mana,
 * Mana Regen Amount,
 * Mana regen interval,
 * --------------------------
 * Charged Spell Cost
 * Charged Spell Damage,
 * Charged Spell Cooldown(Maybe),
 * Charged Spell Charge Up Time
 * --------------------------
 * Melee Damage(Maybe, Could be stun time/Knockback distance),
 * Melee Cooldown,
 * Melee Range
 * --------------------------
 * Max Health
 * 
 */