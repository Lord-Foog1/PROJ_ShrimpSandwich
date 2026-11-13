#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestBoard.generated.h"

// 1. Define the struct ABOVE the class using it
// 2. Add USTRUCT(BlueprintType) and GENERATED_BODY()
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SoulGemReward;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LightCrystalReward;
	
	bool operator==(const FQuest& Other) const
	{
		return QuestName == Other.QuestName;
	}
};

UCLASS()
class PROJ_SHRIMPSANDWICH_API AQuestBoard : public AActor
{
	GENERATED_BODY()
    
public: 
	AQuestBoard();

protected:
	virtual void BeginPlay() override;

public: 
	virtual void Tick(float DeltaTime) override;
    
	// 3. Store FQuest by value, NOT as a pointer (FQuest*)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FQuest> Quests;
    
	UFUNCTION(BlueprintCallable)
	void UpdateQuests();
};