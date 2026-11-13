// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestType.generated.h"

/**
 * 
 */
USTRUCT()
struct FQuestType
{
	GENERATED_BODY();
	
	UPROPERTY()
	FString TimeFrame;
	
	UPROPERTY()
	float HealthRequirement;
	
	UPROPERTY()
	float DamageRequirement;
	
	UPROPERTY()
	int EnemyKills;
};
