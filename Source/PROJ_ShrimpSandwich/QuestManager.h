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
	
#pragma region GETTERS_SETTERS
	
	// Sets what quests exists
	void SetQuests(const TArray<FQuest>& NewQuests);
	
	// Returns one of the arrays
	const TArray<FQuest>& GetQuests() { return Quests; }
	const TArray<FQuest>& GetActiveQuests() { return ActiveQuests; }
	const TArray<FQuest>& GetCompletedQuests() { return CompletedQuests; }
	
	// Returns a specific quest
	FQuest* GetActiveQuest(const FString& QuestName);
	FQuest* GetActiveQuest(const FQuest& Quest) { return GetActiveQuest(Quest.QuestName); }
	
#pragma endregion GETTERS_SETTERS	
	
	void CompleteQuest(const FQuest& Quest);
	
private:
	
	TArray<FQuest> ActiveQuests;
	
	TArray<FQuest> Quests;
	
	TArray<FQuest> CompletedQuests;
	
};
