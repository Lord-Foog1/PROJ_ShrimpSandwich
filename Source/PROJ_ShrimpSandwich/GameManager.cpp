// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManager.h"
#include "Kismet/GameplayStatics.h"

void UGameManager::Init()
{
	Super::Init();
	
	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &UGameManager::OnLevelAddedToWorld);
}

void UGameManager::LoadLevel(TSoftObjectPtr<UWorld> Level)
{
	// Checks if there is a previous level to despawn
	if (!FirstTime)
	{
		PreviousLevel = CurrentLevel;
	}
	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName(*Level.GetAssetName()));
	if (StreamingLevel)
	{
		// Use AddUniqueDynamic so you don't bind it twice if the same level reloads later
		StreamingLevel->bShouldBlockOnLoad = true;
		StreamingLevel->OnLevelShown.AddUniqueDynamic(this, &UGameManager::HandleLevelShown);
	}
	CurrentLevel = Level;
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	
	if(FirstTime || CurrentLevel != PreviousLevel)
	{
		LatentInfo.ExecutionFunction = FName("LevelLoaded");
		LatentInfo.Linkage = 0;
		UGameplayStatics::LoadStreamLevelBySoftObjectPtr(GetWorld(),Level, true, true, LatentInfo);
	}
	else
	{
		LatentInfo.ExecutionFunction = FName("LevelUnloaded");
		LatentInfo.Linkage = 0;
		UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(GetWorld(), Level, LatentInfo, true);
	}
	
}

void UGameManager::OnLevelAddedToWorld(ULevel* InLevel, UWorld* InWorld)
{
	LevelHasBeenAdded.Broadcast();
}

bool UGameManager::GetLevelStarted()
{
	return LevelStarted;
}

TSoftObjectPtr<UWorld> UGameManager::GetCurrentLevel()
{
	return CurrentLevel;
}

TSoftObjectPtr<UWorld> UGameManager::GetPreviousLevel()
{
	return PreviousLevel;
}

void UGameManager::HandleLevelShown()
{
	LevelHasBeenAdded.Broadcast();
}

void UGameManager::LevelLoaded()
{
	if (FirstTime)
	{
		FirstTime = false;
	}
	else
	{
		FLatentActionInfo LatentInfo;
		
		// Deloads previous level
		if (PreviousLevel != CurrentLevel)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *PreviousLevel.ToString());
			UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(GetWorld(), PreviousLevel, LatentInfo, false);
		}
	}
	
	// Lets relevant classes know when a level has finished being loaded in
	OnLevelLoadedInternal.Broadcast();
}

void UGameManager::LevelUnloaded()
{
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("BroadcastLoaded");
	LatentInfo.Linkage = 0;
	
	UGameplayStatics::LoadStreamLevelBySoftObjectPtr(GetWorld(),CurrentLevel, true, false, LatentInfo);
}

