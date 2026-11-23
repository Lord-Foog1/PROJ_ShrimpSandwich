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
		Quests.AddUnique(Quest);
	}
}

void UQuestManager::AcceptQuest(const FQuest& Quest)
{
	if (ActiveQuests.Num() >= 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max number of active quests, can't accept more quests"));
		return;
	}

	if (IsQuestActive(Quest.QuestName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Quest already accepted"));
		return;
	}

	ActiveQuests.Add(Quest);
}

bool UQuestManager::IsQuestActive(const FString& QuestName) const
{
	return ActiveQuests.ContainsByPredicate([&QuestName](const FQuest& Quest)
	{
		return Quest.QuestName == QuestName;
	});
}

bool UQuestManager::GetActiveQuestByName(const FString& QuestName, FQuest& OutQuest) const
{
	const FQuest* FoundQuest = ActiveQuests.FindByPredicate([&QuestName](const FQuest& Quest)
	{
		return Quest.QuestName == QuestName;
	});

	if (!FoundQuest)
	{
		return false;
	}

	OutQuest = *FoundQuest;
	return true;
}

bool UQuestManager::GetActiveQuest(const FQuest& Quest, FQuest& OutQuest) const
{
	return GetActiveQuestByName(Quest.QuestName, OutQuest);
}

void UQuestManager::CompleteQuest(const FQuest& Quest)
{
	FQuest FoundQuest;
	if (!GetActiveQuestByName(Quest.QuestName, FoundQuest))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to complete quest '%s' which is not active"), *Quest.QuestName);
		return;
	}

	ActiveQuests.RemoveAll([&Quest](const FQuest& ActiveQuest)
	{
		return ActiveQuest.QuestName == Quest.QuestName;
	});

	CompletedQuests.AddUnique(FoundQuest);
}