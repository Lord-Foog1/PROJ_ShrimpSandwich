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
	PickQuests();
	if (UGameInstance* Game = GetWorld()->GetGameInstance())
	{
		if (UQuestManager* QM = Game->GetSubsystem<UQuestManager>())
		{
			QM->SetShownQuests(ShownQuests);
		}
	}
}

// Called every frame
void AQuestBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQuestBoard::UpdateQuests()
{
	if (UGameInstance* Game = GetWorld()->GetGameInstance())
	{
		if (UQuestManager* QM = Game->GetSubsystem<UQuestManager>())
		{
			QM->SetQuests(Quests);
		}
	}
}

void AQuestBoard::AcceptQuest(const FQuest& Quest)
{
	if (UGameInstance* Game = GetWorld()->GetGameInstance())
	{
		if (UQuestManager* QM = Game->GetSubsystem<UQuestManager>())
		{
			QM->AcceptQuest(Quest);
		}
	}
}

void AQuestBoard::RerollQuests()
{
	
}

void AQuestBoard::PickQuests()
{
	ShownQuests.Reset();

	if (Quests.Num() == 0)
	{
		NumberOfQuestsOnBoard = 0;
		return;
	}

	NumberOfQuestsOnBoard = FMath::Clamp(NumberOfQuestsOnBoard, 0, Quests.Num());

	TArray<int32> AvailableIndices;
	AvailableIndices.Reserve(Quests.Num());
	for (int32 i = 0; i < Quests.Num(); ++i)
	{
		AvailableIndices.Add(i);
	}

	for (int32 i = 0; i < NumberOfQuestsOnBoard; ++i)
	{
		const int32 RandPos = FMath::RandRange(0, AvailableIndices.Num() - 1);
		const int32 QuestIndex = AvailableIndices[RandPos];
		AvailableIndices.RemoveAtSwap(RandPos);

		ShownQuests.Add(Quests[QuestIndex]);
	}
}