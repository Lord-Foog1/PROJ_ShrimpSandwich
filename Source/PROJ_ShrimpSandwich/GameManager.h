// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "GameManager.generated.h"

class UWidget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelLoadedInternal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelHasBeenAdded);

UCLASS()
class PROJ_SHRIMPSANDWICH_API UGameManager : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
	
public:
	void LoadLevel(TSoftObjectPtr<UWorld> Level);

	FName PendingLevelName;
	
	TSoftObjectPtr<UWorld> PreviousLevel;

	TSoftObjectPtr<UWorld> CurrentLevel;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PauseGame();
	
	UPROPERTY(BlueprintAssignable)
	FOnLevelLoadedInternal OnLevelLoadedInternal;
	
	UPROPERTY(BlueprintAssignable)
	FLevelHasBeenAdded LevelHasBeenAdded;
	
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
	void HandleLevelShown();
	
	UFUNCTION()
	void LevelLoaded();
	
	UFUNCTION()
	void LevelUnloaded();
	
	bool FirstTime = true;
};