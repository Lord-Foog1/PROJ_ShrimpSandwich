// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestBoard.h"

/**
 * 
 */
class PROJ_SHRIMPSANDWICH_API QuestManager : public UGameInstanceSubsystem
{
public:
	QuestManager();
	~QuestManager();
	
	void SetQuests(const TArray<FQuest>& NewQuests) { Quests.Append(NewQuests); }
	const TArray<FQuest>& GetQuests() { return Quests; }
	
private:
	
	TArray<FQuest> ActiveQuests;
	
	TArray<FQuest> Quests;
	
};
