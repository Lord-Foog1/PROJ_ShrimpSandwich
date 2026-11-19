// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestType.generated.h"

UENUM(BlueprintType)
enum class EQuestGoal : uint8
{
	None,
	KillAmount,
	TimeLimit,
	KnockoutLimit,
	PlayerDamageLimit,
	
};

USTRUCT(BlueprintType)
struct FQuestType
{
	GENERATED_BODY();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestGoal GoalType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = 0, ClampMin = 0))
	float LimitAndAmountNumber = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetEnemy = nullptr;
};
