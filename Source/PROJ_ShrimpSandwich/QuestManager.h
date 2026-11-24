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
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void SetQuests(const TArray<FQuest>& NewQuests);

	UFUNCTION(BlueprintCallable, Category = "Quests")
	void AcceptQuest(const FQuest& Quest);

	UFUNCTION(BlueprintCallable, Category = "Quests")
	TArray<FQuest> GetQuests() const { return Quests; }

	UFUNCTION(BlueprintCallable, Category = "Quests")
	const TArray<FQuest>& GetActiveQuests() const { return ActiveQuests; }

	UFUNCTION(BlueprintCallable, Category = "Quests")
	const TArray<FQuest>& GetCompletedQuests() const { return CompletedQuests; }

	UFUNCTION(BlueprintCallable, Category = "Quests")
	bool IsQuestActive(const FString& QuestName) const;

	UFUNCTION(BlueprintCallable, Category = "Quests")
	bool GetActiveQuestByName(const FString& QuestName, FQuest& OutQuest) const;

	UFUNCTION(BlueprintCallable, Category = "Quests")
	bool GetActiveQuest(const FQuest& Quest, FQuest& OutQuest) const;
	
	UFUNCTION(BlueprintCallable, Category = "Quests")
	TArray<FQuest> GetShownQuests() const { return ShownQuests; }
	
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void SetShownQuests(const TArray<FQuest>& NewQuests) { ShownQuests = NewQuests; }

#pragma endregion GETTERS_SETTERS

	UFUNCTION(BlueprintCallable, Category = "Quests")
	void CompleteQuest(const FQuest& Quest);
	
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void AcceptQuests(const FQuest& Quests);

private:
	UPROPERTY()
	TArray<FQuest> ActiveQuests;
	
	UPROPERTY()
	TArray<FQuest> ShownQuests;

	UPROPERTY()
	TArray<FQuest> Quests;

	UPROPERTY()
	TArray<FQuest> CompletedQuests;
};