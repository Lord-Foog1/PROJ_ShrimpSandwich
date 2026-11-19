// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradeManager.h"

#include "MyCharacter.h"


void UUpgradeManager::UpgradeStat(EUpgradeModifierType StatToUpgrade, float DeltaValue, AMyCharacter* TargetCharacter)
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
