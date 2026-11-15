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
	int TimeFrame;
	
	UPROPERTY()
	float DamageTakenRequirement;
	
	UPROPERTY()
	int EnemyKills;
	
	UPROPERTY()
	AActor* EnemyType;
};
