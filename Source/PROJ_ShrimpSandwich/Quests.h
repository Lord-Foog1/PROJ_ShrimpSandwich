// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quests.generated.h"

USTRUCT(BlueprintType)
struct FQuest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString QuestDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "5", UIMin = "1", UIMax = "5"))
	int Difficulty;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", UIMin = "1"))
	int SoulGemReward;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", UIMin = "1"))
	int LightCrystalReward;
	
	bool operator==(const FQuest& Other) const
	{
		return QuestName == Other.QuestName;
	}
	
	friend uint32 GetTypeHash(const FQuest& Quest)
	{
		return GetTypeHash(Quest.QuestName);
	}
};

