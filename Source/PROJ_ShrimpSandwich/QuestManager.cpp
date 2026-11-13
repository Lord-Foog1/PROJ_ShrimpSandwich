// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManager.h"

QuestManager::QuestManager()
{
}

QuestManager::~QuestManager()
{
}

void QuestManager::SetQuests(const TArray<FQuest>& NewQuests)
{
	for (const FQuest& Quest : NewQuests)
	{
		Quests.AddUnique(Quest);
	}
}

FQuest* QuestManager::GetActiveQuest(const FString& QuestName)
{
	return ActiveQuests.FindByPredicate([&QuestName](const FQuest& Quest)
	{
		return Quest.QuestName == QuestName;
	});
}

void QuestManager::CompleteQuest(const FQuest& Quest)
{
	ActiveQuests.RemoveAll([&Quest](const FQuest& ActiveQuest)
	{
		return ActiveQuest.QuestName == Quest.QuestName; 
	});
	
	CompletedQuests.Add(Quest);
}
