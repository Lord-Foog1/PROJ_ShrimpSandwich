// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestManager.h"

UQuestManager::UQuestManager()
{
}

UQuestManager::~UQuestManager()
{
}

void UQuestManager::SetQuests(const TArray<FQuest>& NewQuests)
{
	for (const FQuest& Quest : NewQuests)
	{
		//Quests.AddUnique(Quest);
	}
}

void UQuestManager::AcceptQuest(const FQuest& Quest)
{
	if (ActiveQuests.Num() == 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max number of active quests, can't accept more quests"));
		return;
	}
	if (GetActiveQuest(Quest) != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Quest already accepted"));
		return;
	}
	
	ActiveQuests.Add(Quest);
}

FQuest* UQuestManager::GetActiveQuest(const FString& QuestName)
{
	return ActiveQuests.FindByPredicate([&QuestName](const FQuest& Quest)
	{
		return Quest.QuestName == QuestName;
	});
}

void UQuestManager::CompleteQuest(const FQuest& Quest)
{
	ActiveQuests.RemoveAll([&Quest](const FQuest& ActiveQuest)
	{
		return ActiveQuest.QuestName == Quest.QuestName; 
	});
	
	CompletedQuests.Add(Quest);
}
