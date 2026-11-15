// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestBoard.h"
#include "QuestManager.h"

// Sets default values
AQuestBoard::AQuestBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AQuestBoard::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateQuests();
}

// Called every frame
void AQuestBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQuestBoard::UpdateQuests()
{
	if (UWorld* World = GetWorld())
	{
		if (UQuestManager* QM = World->GetSubsystem<UQuestManager>())
		{
			QM->SetQuests(Quests);
		}
	}
}

void AQuestBoard::AcceptQuest(const FQuest& Quest)
{
	if (UWorld* World = GetWorld())
	{
		if (UQuestManager* QM = World->GetSubsystem<UQuestManager>())
		{
			QM->AcceptQuest(Quest);
		}
	}
}

