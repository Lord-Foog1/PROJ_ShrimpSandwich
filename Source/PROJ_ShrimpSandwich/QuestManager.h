#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Quests.h"
#include "QuestType.h"
#include "QuestManager.generated.h"

UCLASS()
class PROJ_SHRIMPSANDWICH_API UQuestManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UQuestManager();
	virtual ~UQuestManager() override;
    
#pragma region GETTERS_SETTERS
    
	// Sets what quests exist
	void SetQuests(const TArray<FQuest>& NewQuests);
    
	void AcceptQuest(const FQuest& Quest);
    
	// Returns one of the arrays/maps
	TMap<FQuest, FQuestType> GetQuests() const { return Quests; }
	const TArray<FQuest>& GetActiveQuests() const { return ActiveQuests; }
	const TArray<FQuest>& GetCompletedQuests() const { return CompletedQuests; }
    
	// Returns a specific quest pointer safely
	FQuest* GetActiveQuest(const FString& QuestName);
	FQuest* GetActiveQuest(const FQuest& Quest) { return GetActiveQuest(Quest.QuestName); }
    
#pragma endregion GETTERS_SETTERS   
    
	void CompleteQuest(const FQuest& Quest);
    
private:
    
	TArray<FQuest> ActiveQuests;
    
	TMap<FQuest, FQuestType> Quests;
    
	TArray<FQuest> CompletedQuests;
};