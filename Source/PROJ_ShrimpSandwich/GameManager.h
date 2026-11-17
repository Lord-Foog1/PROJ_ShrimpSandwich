// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameManager.generated.h"

class UWidget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelLoadedInternal);

UCLASS()
class PROJ_SHRIMPSANDWICH_API AGameManager : public AGameMode
{
	GENERATED_BODY()
	
public:
	void LoadLevel(TSoftObjectPtr<UWorld> Level);

	FName PendingLevelName;
	
	TSoftObjectPtr<UWorld> PreviousLevel;

	TSoftObjectPtr<UWorld> CurrentLevel;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PauseGame();
	
	UPROPERTY()
	FOnLevelLoadedInternal OnLevelLoadedInternal;
	
	bool GetLevelStarted();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSoftObjectPtr<UWorld> GetCurrentLevel();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSoftObjectPtr<UWorld> GetPreviousLevel();
	
protected:
	UPROPERTY(BlueprintReadWrite)
	bool LevelStarted;

private:
	UFUNCTION()
	void LevelLoaded();
	
	UFUNCTION()
	void LevelUnloaded();

	UFUNCTION()
	void BroadcastLoaded();

	bool FirstTime = true;
};