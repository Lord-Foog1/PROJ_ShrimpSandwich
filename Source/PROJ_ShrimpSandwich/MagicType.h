// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMagicType : uint8
{
	Fire	UMETA(DisplayName = "Fire"),
	Water	UMETA(DisplayName = "Water"),
	Nature	UMETA(DisplayName = "Nature"),
	Wind	UMETA(DisplayName = "Wind"),
};