#pragma once

#include "CoreMinimal.h"
#include "Quests.h"
#include "GameFramework/Actor.h"
#include "QuestBoard.generated.h"

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (ShowOnlyInnerProperties))
	TArray<FQuest> Quests;
    
	UFUNCTION(BlueprintCallable)
	void UpdateQuests();
	
	UFUNCTION(BlueprintCallable)
	void AcceptQuest(const FQuest& Quest);
	
};