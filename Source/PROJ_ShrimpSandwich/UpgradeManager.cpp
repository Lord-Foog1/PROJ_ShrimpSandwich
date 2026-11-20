// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradeManager.h"

#include "MyCharacter.h"
#include "Spell.h"

void UUpgradeManager::UpgradePlayerStat(EUpgradeModifierType StatToUpgrade, float DeltaValue, AMyCharacter* TargetCharacter)
{
	if (!TargetCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpgradeStat: TargetCharacter is null."));
		return;
	}

	if (TargetCharacter->HasAuthority())
	{
		TargetCharacter->ApplyUpgradeStat(StatToUpgrade, DeltaValue);
	}
	else
	{
		TargetCharacter->Server_UpgradeStat(StatToUpgrade, DeltaValue);
	}
}

void UUpgradeManager::UpgradeSpellStat(EUpgradeModifierType StatToUpgrade, float DeltaValue, ASpell* TargetSpell)
{
	if (!TargetSpell)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpgradeStat: TargetSpell is null."));
		return;
	}
	
	if (TargetSpell->GetOwner()->HasAuthority())
	{
		TargetSpell->ApplyUpgradeStat(StatToUpgrade, DeltaValue);
	}
	else
	{
		TargetSpell->Server_UpgradeStat(StatToUpgrade, DeltaValue);
	}
}
