#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Quests.h"
#include "QuestType.h"
#include "QuestManager.generated.h"

UCLASS()
class PROJ_SHRIMPSANDWICH_API UQuestManager : public UGameInstanceSubsystem
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
	TArray<FQuest> GetQuests() const { return Quests; }
	const TArray<FQuest>& GetActiveQuests() const { return ActiveQuests; }
	const TArray<FQuest>& GetCompletedQuests() const { return CompletedQuests; }
    
	// Returns a specific quest pointer safely
	FQuest* GetActiveQuest(const FString& QuestName);
	FQuest* GetActiveQuest(const FQuest& Quest) { return GetActiveQuest(Quest.QuestName); }
    
#pragma endregion GETTERS_SETTERS   
    
	void CompleteQuest(const FQuest& Quest);
    
private:
    
	TArray<FQuest> ActiveQuests;
    
	TArray<FQuest> Quests;
    
	TArray<FQuest> CompletedQuests;
};

/**
 * 
 * Quest:
 * Name,
 * Description,
 * Reward,
 * Difficulty
 * 
 * Quest Types:
 * Goal of Quests?
 * Time Limit,
 * kill Count,
 * Player Damage Limit,
 * Player Knockout Limit
 * 
 * QuestManager:
 * Store Quests (Get/Set/Add),
 * Store Active Quests (Get/Set/Add),
 * Store Completed Quests (Get/Set/Add)
 * 
 * Quest Functions:
 * Accept Quests,
 * Reroll Quests,
 * Complete Quests and Get Rewards,
 * Change How The Round Functions
 * 
 */